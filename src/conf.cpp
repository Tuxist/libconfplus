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

confplus::Config::ConfigValue::ConfigValue(){
    _nextValue=nullptr;
}

confplus::Config::ConfigValue::~ConfigValue(){
    delete _nextValue;
}

confplus::Config::ConfigData *confplus::Config::getKey(const char* key){
    ConfigData *cdat=firstData;
    if(key[0]!='/'){
        ConfException exp;
        exp[ConfException::Error] << "Config: getkey wrong path!";
        throw exp;
    }
    size_t start=1,pos=1;
    while(pos<=strlen(key)){
GETKEYSEARCH:
        if(key[pos]=='/' || key[pos]=='\0'){
            std::string childkey;
            std::copy(key+start,key+pos,std::inserter<std::string>(childkey,childkey.begin()));
            while(cdat){
                if(cdat->Key==childkey){
                    if(key[pos]=='\0'){
                          return cdat;
                    }
                    cdat=cdat->Child;
                    ++pos;
                    start=pos;
                    goto GETKEYSEARCH;
                }
                cdat=cdat->nextData;
            }
        }
        ++pos;
    }
    ConfException exp;
    exp[ConfException::Error] << "Config: key not found";
    throw exp;
}

confplus::Config::ConfigData *confplus::Config::setKey(const char* key){

    auto createdat = [] (ConfigData **root){
        while(*root){
            root=&(*root)->nextData;
        }
        *root=new ConfigData;
        return *root;
    };

    auto existsdat = [] (ConfigData *search,const std::string ckey){
        while(search){
            if(search->Key==ckey){
                return search;
            }
            search=search->nextData;
        }
        return (ConfigData*)nullptr;
    };

    if(key[0]!='/'){
        ConfException exp;
        exp[ConfException::Error] << "Config: setkey wrong path!";
        throw exp;
    }

    size_t start=1,pos=1;

    ConfigData *find=nullptr,**child=&firstData;

    while(pos<=strlen(key)){
        if(key[pos]=='/' || key[pos]=='\0'){

            std::string childkey;
            std::copy(key+start,key+pos,std::inserter<std::string>(childkey,childkey.begin()));
            start=(pos+1);

            find=existsdat(*child,childkey);

            if(!find){
                find = createdat(child);
                find->Key=childkey;
                if(key[pos]!='\0')
                    find->haveChild=true;
            }
            child=&find->Child;
        }
        ++pos;
    }

    return find;
}

void confplus::Config::delKey(confplus::Config::ConfigData* key){
    ConfigData *bef=nullptr;
    for(ConfigData *cur=firstData; cur; cur=cur->nextData){
        if(cur==key){
            bef->nextData=key->nextData;
            key->nextData=nullptr;
            delete key;
        }
        bef=cur;
    }
}


confplus::Config::ConfigData::ConfigData(){
    Value=nullptr;
    nextData=nullptr;
    haveChild=false;
}

confplus::Config::ConfigData::~ConfigData(){
    if(haveChild)
        delete Child;
    else
        delete Value;

    delete nextData;
}

size_t confplus::Config::ConfigData::getElements(){
    return Elements;
}

confplus::Config::Config(const char* path){
    firstData=nullptr;

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
        err[ConfException::Critical] << "Cannot load symbol create: " << dlsym_error << '\n';
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

size_t confplus::Config::getElements(confplus::Config::ConfigData* key){
    return key->Elements;
}

const char * confplus::Config::getValue(confplus::Config::ConfigData* key, size_t pos){
    if(key->haveChild){
        ConfException err;
        err[ConfException::Error] << "getValue it is a path not key" << pos  << '\n';
        throw err;
    }
    if(key->Elements<pos){
        ConfException err;
        err[ConfException::Error] << "getValue pos out of range !" << pos  << '\n';
        throw err;
    }

    size_t lvl=0;

    for(ConfigValue *cur=key->Value; cur; cur=cur->_nextValue){
        if(lvl==pos){
            return cur->_Value.c_str();
        }
        ++lvl;
    }

    return nullptr;

}

int confplus::Config::getIntValue(confplus::Config::ConfigData* key, size_t pos){
    return atoi(getValue(key,pos));
}

void confplus::Config::setValue(confplus::Config::ConfigData* key, size_t pos, const char* value){
    for(ConfigValue *cur=key->Value; cur; cur=cur->_nextValue){
        if(cur->_Pos==pos){
            cur->_Value=value;
            return;
        }
    }

    ConfigValue *cur=key->Value,*bef=nullptr;

    while(cur){
        bef=cur;
        cur=cur->_nextValue;
    };

    if(!bef){
        bef = new ConfigValue();
    }

    bef->_nextValue = new ConfigValue();
    bef->_Value = value;
    bef->_Pos = pos;

    ++key->Elements;

    return;
}

void confplus::Config::setIntValue(confplus::Config::ConfigData* key, size_t pos, int value){
   char inv[512];
   snprintf(inv,512,"%d",value);
   setValue(key,pos,inv);
}
