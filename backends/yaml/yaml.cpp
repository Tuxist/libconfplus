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

#include "conf.h"
#include "yaml.h"

confplus::Yaml::Yaml(){

}

confplus::Yaml::~Yaml(){
}


// confplus::ConfigData * confplus::Yaml::_scanBlock(yaml_token_t* curblock,yaml_token_t *next,ConfigData *parent,ConfigData *cur){
    // /* BEGIN new code */
    // if (curblock->type != YAML_STREAM_END_TOKEN){
    //     switch(curblock->type) {
    //         case(YAML_BLOCK_MAPPING_START_TOKEN):{
    //             if(!parent){
    //                 cur = new ConfigData;
    //                 cur->Key.insert(0,(const char*)curblock->data.scalar.value,curblock->data.scalar.length);
    //             }else{
    //                 cur->childData = new ConfigData;
    //                 parent=cur->childData;
    //             }
    //         }break;
    //         case(YAML_BLOCK_END_TOKEN):{
    //             return parent;
    //         }break;
    //         case YAML_VALUE_TOKEN:{
    //             if(conf.firstData){
    //                 lastData->nextData= new ConfigData;
    //                 lastData=lastData->nextData;
    //             }else{
    //                 firstData= new ConfigData;
    //                 lastData=firstData;
    //             }
    //         }break;
    //         default:
    //             printf("Got token of type %d\n", curblock->type);
    //     }
    // }
    // return parent;
// }

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
    FILE *fh = fopen(path,"r");
    yaml_token_t  token,token2;

     /* Initialize parser */
    if(!yaml_parser_initialize(&_Parser))
        throw "Failed to initialize parser!";
    if(fh == nullptr)
        throw "Failed to open file!";

    /* Set input file */
    yaml_parser_set_input_file(&_Parser, fh);

    yaml_parser_scan(&_Parser, &token);
    yaml_parser_scan(&_Parser, &token2);

    if(conf->firstData){
        delete conf->firstData;
        conf->firstData=nullptr;
    }

    // conf->lastData=_scanBlock(&token,&token2,nullptr,conf->firstData);


    yaml_token_delete(&token);
    yaml_token_delete(&token2);

    yaml_parser_delete(&_Parser);
    fclose(fh);
}
