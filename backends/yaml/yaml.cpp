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

#include <string>

#include "conf.h"
#include "yaml.h"

namespace confplus {
    struct YamlStack {
        std::string   anchor;
        YamlStack    *prevel;
    };
};

confplus::Yaml::Yaml(){

}

confplus::Yaml::~Yaml(){
}

const char* confplus::Yaml::getName(){
    return "yaml";
}

const char* confplus::Yaml::getVersion(){
    return "0.1";
}

const char* confplus::Yaml::getAuthor(){
    return "Jan Koester";
}

void confplus::Yaml::saveConfig(const char *path,const Config *conf){

}


void confplus::Yaml::loadConfig(const char *path,Config *conf){
    FILE *fh = fopen(path,"rb");
    yaml_event_type_t type;
    yaml_parser_t parse;

     /* Initialize parser */
    if(!yaml_parser_initialize(&parse))
        throw "Failed to initialize parser!";
    if(fh == nullptr)
        throw "Failed to open file!";

    /* Set input file */
    yaml_parser_set_input_file(&parse, fh);

    YamlStack *ystack=nullptr;

    bool seq=false;
    std::string key,value;
    int pos = 0;
    do {
        yaml_event_t event;
        if (!yaml_parser_parse(&parse, &event))
            break;

        switch (event.type) {
            case YAML_MAPPING_START_EVENT: {
                    YamlStack *cystack=new YamlStack;
                    cystack->prevel=ystack;
                    ystack=cystack;
                    ystack->anchor=key;
                    key=value;
                    value.clear();
            }break;

            case YAML_MAPPING_END_EVENT:{
                if(ystack->prevel){
                    YamlStack *pystack=ystack->prevel;
                    delete ystack;
                    ystack=pystack;
                }
            }break;


            case YAML_SCALAR_EVENT:{
                if(key.empty())
                    std::copy(event.data.scalar.value,event.data.scalar.value+event.data.scalar.length,std::inserter<std::string>(key,std::begin(key)));
                else
                    std::copy(event.data.scalar.value,event.data.scalar.value+event.data.scalar.length,std::inserter<std::string>(value,std::begin(value)));
            }break;

            case YAML_SEQUENCE_START_EVENT:{
                seq=true;
            }break;

            case YAML_SEQUENCE_END_EVENT:{
                seq=false;
                key=value;
                pos=0;
                value.clear();
            }break;

            default:
                break;
        }

        if(!key.empty() && !value.empty()){
            std::string cname;
            for(YamlStack *curst=ystack; curst; curst=curst->prevel){
                std::string pp=curst->anchor; pp+='/';
                std::copy(pp.begin(),pp.end(),std::inserter<std::string>(cname,std::begin(cname)));
            }
            cname+=key;
            Config::ConfigData *ckey=conf->setKey(cname.c_str());
            conf->setValue(ckey,pos,value.c_str());
            value.clear();
            if(!seq){
                key.clear();
            }else{
                ++pos;
            }
        }
        type=event.type;
        yaml_event_delete(&event);
    } while (type!= YAML_STREAM_END_EVENT);

    yaml_parser_delete(&parse);
    fclose(fh);
}
