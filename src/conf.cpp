/*******************************************************************************
 * Copyright (c) 2023, Jan Koester jan.koester@gmx.net
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#include <iostream>
#include <algorithm>

#include <cstdio>
#include <string>
#include <cstring>

#include <dlfcn.h>

#include "exception.h"
#include "conf.h"
#include "config.h"

confplus::ConfigValue::ConfigValue(){
    _firstValue=nullptr;
    _nextValue=nullptr;
    _Index=0;
}

confplus::ConfigValue::~ConfigValue(){
    delete _nextValue;
}

const char * confplus::ConfigValue::operator[](size_t idx){
    for(ConfigValue *cval=_firstValue; cval; cval=cval->_nextValue){
        if(cval->_Index==idx)
            return _Value.c_str();
    }
    return nullptr;
}

confplus::ConfigValue & confplus::Config::Value(const char* key){
    for(ConfigData *cdat=firstData; cdat; cdat=cdat->nextData){
        if(cdat->Key==key){
            return cdat->Value;
        }
    }
    ConfException exp;
    exp[ConfException::Critical] << "Config: key not found";
    throw exp;
}

confplus::Config::ConfigData & confplus::Config::Child(const char* key){
    for(ConfigData *cdat=firstData; cdat; cdat=cdat->nextData){
        if(cdat->Key==key){
            return *cdat->childData;
        }
    }
    ConfException exp;
    exp[ConfException::Critical] << "Config: key not found";
    throw exp;
}

confplus::Config::ConfigData::ConfigData(){
    childData=nullptr;
    nextData=nullptr;
}

confplus::Config::ConfigData::~ConfigData(){
    delete childData;
    delete nextData;
}

size_t confplus::Config::ConfigData::getElements(){
    return Elements;
}

confplus::Config::Config(const char* path){
    firstData=nullptr;
    lastData=nullptr;

    int i =0;

    std::string mpath=CONFIGPATH;

    mpath+="/";

    while(path[i++]!=':');

    std::copy(path,path+(i-1),std::inserter<std::string>(mpath,mpath.end()));

    mpath+=".so";

    ++i;

    std::string confpath;

    std::copy(path+i,path+strlen(path),std::inserter<std::string>(confpath,confpath.begin()));

    _BackendData = dlopen(mpath.c_str(),RTLD_LAZY);
    if (!_BackendData) {
        ConfException err;
        err[ConfException::Critical] << "Cannot load library: " << dlerror();
        throw err;
    }
    // load the symbols
    create_t* create_plugin= (create_t*) dlsym(_BackendData, "create");
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        ConfException err;
        err << "Cannot load symbol create: " << dlsym_error << '\n';
        throw err;
    }


    dlerror();

    _currApi=create_plugin();
    _currApi->loadConfig(confpath.c_str(),this);
}

confplus::Config::~Config(){
        const char* dlsym_error = dlerror();
        destroy_t* destroy_plugin = (destroy_t*) dlsym(_BackendData, "destroy");
        dlsym_error = dlerror();
        if (dlsym_error) {
            std::cerr << "Cannot load symbol destroy: " << dlsym_error << std::endl;
        }

        destroy_plugin(_currApi);

        dlclose(_BackendData);
        delete firstData;
}

const char* confplus::Config::getName(){
    return _currApi->getName();
}

const char* confplus::Config::getVersion(){
    return _currApi->getVersion();
}

const char* confplus::Config::getAuthor(){
    return _currApi->getAuthor();
}

void confplus::Config::saveConfig(const char *path){
    _currApi->saveConfig(path,this);
}
