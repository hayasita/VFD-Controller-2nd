//import {json} from '../../data.js';

export class vfdControllerDomain{
    constructor(initialSetting){
        console.log("-- vfdControllerDomain : initialSetting --");
//        console.log(this);
//        console.log(initialSetting);
        this.jsonObj = JSON.parse(initialSetting);
        this.resetJsonTmpData();
//        console.log(this.jsonObj);
//        console.log("initialSetting");
    }
    // -- センサ情報コールバック設定 --
    setSensorDataCallback(callbackFunc){
        console.log("-- setSensorDataCallback : Callback Setting --")
        this._callbackFuncSensorData = callbackFunc;
    }
    // -- 時刻情報コールバック設定 --
    setTimeDataCallback(callbackFunc){
        console.log("-- setTimeDataCallback : Callback Setting --")
        this._callbackFuncTimaData = callbackFunc;
    }
    // -- WiFiStation情報コールバック設定 --
    setWiFiStationListCallback(callbackFunc){
        console.log("-- setWiFiStationListCallback : Callback Setting --")
        this._callbackFuncWiFiStationList = callbackFunc;
    }
    // -- WebSocket情報コールバック設定 --
    setWebSocketDataCallback(callbackFunc){
        console.log("-- setWebSocketDataCallback : Callback Setting --")
        this._callbackFuncWebSocketData = callbackFunc;
    }
    // -- WebSocket送信コールバック設定 --
    setWebsocketSendCallback(callbackFunc){
      console.log("-- setWebsocketSendCallback : Callback Setting --");
      this._callbackFuncWebsocketSend = callbackFunc;
    }

    // --  WebSocket初期化 --
    websocketInit(){
        console.log("websocketInit : webSocketSet:");
//        this.ws = new WebSocket('ws://' + window.location.hostname + ':81/');
        this.ws = new WebSocket(`ws://${location.hostname}/ws`);
        this.ws.onmessage = (evt) => {
//            console.log("webSocket onmessage:");
    //        console.log(evt.data);
          let objData = JSON.parse(evt.data);
    //        console.log(objData.stationList);
//            console.log(objData);
//            console.log(Object.keys(objData['sensor']).length);
//            console.log(objData.sensor);
//            console.log(objData.sensor[0]);
//            let sensorDat = [];
//            for(let i in objData.sensor){
//                console.log(objData.sensor[i]);
//                sensorDat.push(objData.sensor[i]);
//            }
//            var Time = new Date().toLocaleTimeString();

          if(!(typeof objData.sensor === "undefined")){
            this._callbackFuncSensorData(objData);    // 受信データ処理コールバック
          }
          if(!(typeof objData.time === "undefined")){
            this._callbackFuncTimaData(objData.time);
          }
          this._callbackFuncWebSocketData(objData);
          if(!(typeof objData.stationList === "undefined")){
            this._callbackFuncWiFiStationList(objData.stationList);
          }
        };
        this.ws.onclose = function(evt) {
          console.log("ws: onclose");
//          let objData = "{\"websocket\" : \"close\"}";
//          this._callbackFuncWebsocketSend(objData);
        }
        this.ws.onerror = function(evt) {
          console.log("ws: onerror");
          console.log(evt);
        }
        this.ws.onopen = (evt) => {
          console.log("ws: onopen");
          let objData = "{\"websocket\" : \"open\"}";
          this._callbackFuncWebsocketSend(objData);
        };
    }
    // --  WebSocket データ送信 --
    websocketSend(sendData){
      console.log("--websocketSend");
      console.log(sendData);
      console.log("WebSocket.readyState:"+this.ws.readyState);
      let ret = this.ws.readyState;
      if(ret == "1"){
        this.ws.send(sendData);
      }
      else{
        console.log("Cannot Send data.");
      }
        return ret;
    }
    resetJsonTmpData(){
      if(this.jsonObj.hasOwnProperty('brDig')){
        this.jsonObj.brDigtmp = Array.from(this.jsonObj.brDig);             // brDigtmp作成
      }
      else{
        this.jsonObj.brDig = Array.from([1,2,3,4,5,6,7,8,9]);
        this.jsonObj.brDigtmp = Array.from(this.jsonObj.brDig);
      }
      if(this.jsonObj.hasOwnProperty('glowInTheBright')){
        this.jsonObj.glowInTheBrighttmp = this.jsonObj.glowInTheBright;     // 表示輝度設定画面表示用初期値
      }
      if(this.jsonObj.hasOwnProperty('glowInTheDark')){
        this.jsonObj.glowInTheDarktmp = this.jsonObj.glowInTheDark;         // 表示輝度設定画面表示用初期値
      }
    }
    resetBrSetting(){
        console.log("resetBrSetting");
        this.jsonObj.brDigtmp = Array.from(this.jsonObj.brDig);
//        this.jsonObj.fadeTimetmp = this.jsonObj.fadeTime;
//        this.jsonObj.formatHourtmp = this.jsonObj.formatHour;
//        this.jsonObj.glowBrighttmp = this.jsonObj.glowBright;
//        this.jsonObj.glowDarktmp = this.jsonObj.glowDark;
    }
    writeBrSetting(){
        console.log("writeBrSetting");
        this.jsonObj.brDig = Array.from(this.jsonObj.brDigtmp);
//        this.jsonObj.fadeTime = this.jsonObj.fadeTimetmp;
//        this.jsonObj.formatHour = this.jsonObj.formatHourtmp;
//        this.jsonObj.glowBright = this.jsonObj.glowBrighttmp;
//        this.jsonObj.glowDark = this.jsonObj.glowDarktmp;

    }

    // setting.js JSON情報取得
    getSettingJsonItem(item, ...[num]){
        let data;
        if(typeof num === "number"){
            data = this.jsonObj[item][num-1];
        }else{
            data = this.jsonObj[item];
        }
//        console.log("getSettingJsonItem:" + item + ":" + data);
        if(typeof data === 'undefined'){
            data = 0;
        }
        return data;
    }
    // setting.js JSON情報設定
    setSettingJsonItem(item, data, ...[num]){
      console.log("- setSettingJsonItem -");
      let ret;
      if(typeof num === "number"){
        this.jsonObj[item][num-1] = data;
//        console.log(this.jsonObj[item]);
        ret = this.jsonObj[item][num-1];
      }
      else{
        this.jsonObj[item] = data;
        ret = this.jsonObj[item];
      }
      console.log(this.jsonObj[item]);
//        console.log(this.jsonObj);
      return ret;
    }

    // -- JSONデータ POST送信 --
    jsonPost(sendData){
        var xhr = new XMLHttpRequest();
//        xhr.onreadystatechange = function(){
        xhr.onreadystatechange =(evt) =>{
            if (xhr.readyState == 4){
                if (xhr.status == 200){
                // 受信したデータの処理を記述する
                    console.log("200!")
                }
            }
        }

        xhr.open('POST', '/');
        xhr.setRequestHeader('content-type', 'application/JSON;charset=UTF-8');
        xhr.send( sendData );

        return;
    }

}
