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

#include <string>

#include "conf.h"
#include "yaml.h"

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
    FILE *fh = fopen(path,"r");
    yaml_token_t  token;
    yaml_event_t event;
    yaml_event_type_t event_type;

    int curlevel=0;

     /* Initialize parser */
    if(!yaml_parser_initialize(&_Parser))
        throw "Failed to initialize parser!";
    if(fh == nullptr)
        throw "Failed to open file!";

    /* Set input file */
    yaml_parser_set_input_file(&_Parser, fh);


    do {
        if (!yaml_parser_parse(&_Parser, &event))
            break;
        switch (event.type) {
            case YAML_SCALAR_EVENT:{
            }break;

        }
        event_type=event.type;
        yaml_event_delete(&event);

    } while (event_type!= YAML_STREAM_END_EVENT);

    yaml_token_delete(&token);
    yaml_parser_delete(&_Parser);
    fclose(fh);
}
