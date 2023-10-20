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
#include <stdio.h>

#include "backend.h"

#pragma once

namespace confplus {

    class Config {
    public:
        Config(const char *path);
        ~Config();

        class ConfigValue{
        private:
            ConfigValue();
            ~ConfigValue();
            std::string   _Value;
            size_t        _Pos;
            ConfigValue  *_nextValue;
            friend class Config;
        };

        class ConfigData{
        private:
            size_t getElements();
            ConfigData();
            ~ConfigData();

            std::string  Key;
            size_t       Elements;

            bool         haveChild;

            union {
                ConfigValue *Value;
                ConfigData  *Child;
            };

            ConfigData  *nextData;
            friend class Config;
        };


        ConfigData  *getKey(const char* key);
        ConfigData  *setKey(const char *key);
        void         delKey(ConfigData  *key);

        size_t getElements(ConfigData *key);

        void setValue(ConfigData *key,size_t pos,const char *value);
        void setIntValue(ConfigData *key,size_t pos,int value);

        const char* getValue(ConfigData *key,size_t pos);
        int  getIntValue(ConfigData *key,size_t pos);

        ConfigData *firstData;
    public:
        const char* getName();
        const char* getVersion();
        const char* getAuthor();

        void saveConfig(const char *path);
    private:
        BackendApi *_currApi;
        void       *_BackendData;
    };
};
