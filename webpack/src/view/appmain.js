import '@riotjs/hot-reload'
import {component} from 'riot'
import appmain from './app-main.riot'
import {vfdControllerUI} from '../viewmodel/vmapp.js'
import observable from '@riotjs/observable'

export function StartModelSelection(){
    new ModelselectionApp();
}

class ModelselectionApp{
    constructor(){
        console.log("ModelselectionApp.constructor")
        this.UI = new vfdControllerUI(_initial_setting_);
        let vmUI = this.UI;

        // websocket処理初期化
        vmUI.websocketInit();

        let obs = observable(this);

        component(appmain)(document.getElementById('appmain'), {vmUI, obs})
    }
}
