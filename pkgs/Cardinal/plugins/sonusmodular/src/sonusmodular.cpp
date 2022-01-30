/******************************************************************************
 * Copyright 2017-2019 Valerio Orlandini / Sonus Modular <SonusModular@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/


#include "sonusmodular.hpp"


Plugin *pluginInstance;

void init(rack::Plugin *p)
{
    pluginInstance = p;
    
    p->addModel(modelAddiction);
    p->addModel(modelBitter);
    p->addModel(modelBymidside);
    p->addModel(modelCampione);
    p->addModel(modelChainsaw);
    p->addModel(modelCtrl);
    p->addModel(modelDeathcrush);
    p->addModel(modelFraction);
    p->addModel(modelHarmony);
    p->addModel(modelLadrone);
    p->addModel(modelLuppolo);
    p->addModel(modelLuppolo3);
    p->addModel(modelMicromacro);
    p->addModel(modelMrcheb);
    p->addModel(modelMultimulti);
    p->addModel(modelNeurosc);
    p->addModel(modelOktagon);
    p->addModel(modelOsculum);
    p->addModel(modelParamath);
    p->addModel(modelPiconoise);
    p->addModel(modelPith);
    p->addModel(modelPusher);
    p->addModel(modelRingo);
    p->addModel(modelScramblase);
    p->addModel(modelTropicana);
    p->addModel(modelTwoff);
    p->addModel(modelYabp);
}
