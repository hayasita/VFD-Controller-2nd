//import { websocketSend, webSocketSet, vfdControllerDomain } from "../model/vfdControllerDomain";
import {  vfdControllerDomain } from "../model/vfdControllerDomain";
//import { setSensorData } from './view/dashbord.tag';
import {ModelselectionSetting} from './setting'

export class vfdControllerUI{
    constructor(initialSetting){
        this.model = new vfdControllerDomain(initialSetting);
        this.setting = new ModelselectionSetting(this.model);

        //console.log(this.model.timezone);

        this.model.setSensorDataCallback(this.getSensorData.bind(this));
        this.model.setTimeDataCallback(this.getTimeData.bind(this));
        // Websocket取得コールバック設定
        this.model.setWebSocketDataCallback(this.getWebsocketData.bind(this));
        // WiFiStaList取得　コールバック設定
        this.model.setWiFiStationListCallback(this.getWifiList.bind(this));
        // WebSocket送信コールバック設定
        this.model.setWebsocketSendCallback(this.submitWebsocket.bind(this));
        
        // LogMessage初期化
        this.logdata = "Log start.<br>";
        this.sntpLogdata = "SNTP Log start.<br>"
        console.log(this.logdata);
        this.eventLogName = this.model.eventName;
/*        this.eventLogName = {
          0x10:"0x10:EVENT_BOOT_TASKDEVICECTRL",
          0x11:"0x11:イベント遅延書き込み",
          0x20:"0x20:RTC時刻設定", 0x21:"0x21:SNTP処理要求",
          0x22:"0x22:SNTP取得 設定時刻接続動作要求",0x23:"0x23:SNTP取得 接続動作完了",
          0x24:"0x24:SNTP取得 起動時接続動作要求",
          0x80:"0x80:WiFi自動接続", 0x81:"0x81:WiFi自動接続完了",
          0x82:"0x82:SNTP処理完了", 0x83:"0x83:SNTP処理タイムアウト",
          0x84:"0x84:手動接続・WiFi APモード起動", 0x85:"0x85:手動接続・WiFi APモード起動失敗",
          0x86:"0x86:手動接続・WiFi APモード起動完了", 0x87:"0x87:手動接続・WiFi STA起動",
          0x88:"0x88:手動接続・WiFi STA接続完了", 0x89:"0x89:STA再接続・切断",
          0x8A:"0x8A:STA再接続・接続", 0x8B:"0x8B:SSID削除",
          0x8C:"0x8C:STA再接続・接続完了", 0x8D:"0x8D:WiFI切断",
          0x8E:"0x8E:STA接続エラー・タイムアウト", 0x8F:"0x8F:WiFI手動中断",
          0xA0:"0xA0:WiFi SSID 検索", 0xA1:"0xA1:WiFi SSID 検索完了"
        };
*/
        var SenserDataX1;
        var SenserDataX2;
    }
    websocketInit(sensorDataSet){
        this.model.websocketInit();
    }
    //
    setNtpLastUpdateTImeCallback(callbackFunc){
        console.log("-- vmapp : NtpLastUpdateTImeCallback : Callback Setting --")
        this._callbackFuncNtpLastUpdateTIme = callbackFunc;
    }
    // WiFi Station 接続情報更新　コールバック設定
    updateStaConnStsCallback(callbackFunc){
        console.log("-- vmapp : StaSsidUpdateCallback : Callback Setting --")
        this._callbackFuncUpdateStaConnSts = callbackFunc;
    }
    // DashBord EventLog 更新コールバック設定
    updateDashBordEventLogCallback(callbackFunc){
        console.log("-- vmapp : updateDashBordEventLogCallback : Callback Setting --")
        this._callbackFuncDashBordEventLog = callbackFunc;
    }
    // WiFi EventLog 更新コールバック設定
    updateEventLogCallback(callbackFunc){
        console.log("-- vmapp : updateEventLogCallback : Callback Setting --")
        this._callbackFuncWiFiEventLog = callbackFunc;
    }
    // SNTP SettingLog 更新コールバック設定
    updateSntpEventLogCallback(callbackFunc){
        console.log("-- vmapp : updateSntpEventLogCallback : Callback Setting --")
        this._callbackFuncSNTPEventLog = callbackFunc;
    }
    // SNTP 手動更新完了コールバック設定
    clearSntpSpinnerCallback(callbackFunc){
        console.log("-- vmapp : clearSntpSpinnerCallback : Callback Setting --")
        this._callbackFuncClearSntpSpinner = callbackFunc;
    }
    // WiFi STA接続更新完了コールバック設定
    clearStaReConnectionCallback(callbackFunc){
        console.log("-- vmapp : clearStaReConnectionCallback : Callback Setting --")
        this._callbackFuncClearStaReConnection = callbackFunc;
    }
    //
    getWebsocketData(websocketData){
        if(!(typeof websocketData.lastUpdate === "undefined")){
            console.log("CallBack getWebsocketData lastUpdate : ");
            console.log(websocketData.lastUpdate);
            this._callbackFuncNtpLastUpdateTIme(websocketData.lastUpdate);
        }
        // WiFi Station 接続情報更新コールバック
        if(!(typeof websocketData.staSsid === "undefined")){
            console.log("CallBack getWebsocketData staSsid : ");
            console.log(websocketData.staSsid);
            this._callbackFuncUpdateStaConnSts(websocketData);
        }
        // WiFi EventLog 更新コールバック
        if(!(typeof websocketData.eventLog === "undefined")){
            console.log("CallBack getWebsocketData eventLog : ");
            console.log(websocketData.eventLog);
            console.log(this.logdata);
            // Log作成
            let logTmp = "";
            let eventName = this.eventLogName;
            websocketData.eventLog.forEach(function (obj){
                let log = obj.year + "/" + obj.month + "/" + obj.day + " " + obj.hour + ":" + obj.min + ":" + obj.sec + " " + eventName[obj.event];
                log = log + ":" + obj.data[0] + ":" + obj.data[1] + ":" + obj.data[2] + ":" + obj.data[3] + ";";
                logTmp += log + "<br>";
            });
            console.log(logTmp);
            this.logdata += logTmp;
            this._callbackFuncWiFiEventLog(this.logdata);
            this._callbackFuncDashBordEventLog(this.logdata);

            // SNTP Log
            let sntpComp = false;
            let staReconnentComp = false;
            logTmp = "";
            websocketData.eventLog.forEach(function (obj){
                if(((obj.event >= 0x20) && (obj.event <= 0x24)) || (obj.event == 0x82) || (obj.event == 0x83)){
                    const log = obj.year + "/" + obj.month + "/" + obj.day + " " + obj.hour + ":" + obj.min + ":" + obj.sec + " " + eventName[obj.event];
                    logTmp += log + "<br>";
                }
                if((obj.event == 0x82)){      // Event:SNTP処理完了
                    sntpComp = true;
                    console.log("-- SNTP Comp Spinner Clear. --");
                }
                if((obj.event == 0x8C)){      // Event:STA再接続・接続完了
                    staReconnentComp = true;
                    console.log("-- STA Reconnect Comp Spinner Clear. --");
                }
            });
            console.log(logTmp);
            this.sntpLogdata += logTmp;
            this._callbackFuncSNTPEventLog(this.sntpLogdata);
            if(sntpComp){
                this._callbackFuncClearSntpSpinner();
            }
            if(staReconnentComp){
                this._callbackFuncClearStaReConnection();
            }

        }

    }
    // -- WiFi Station List 取得　コールバック設定 --
    setWiFiStationListCallback(callbackFunc){
      console.log("-- vmapp : getWiFiListCallback : Callback Setting --")
      this._callbackFuncWiFiStationList = callbackFunc;
    }
    getWifiList(wifiStaList){
      console.log("CallBack getWifiList : ");
      console.log(wifiStaList);
      this._callbackFuncWiFiStationList(wifiStaList);
    }
    // -- 時刻情報コールバック設定 --
    setTimeDataCallback(callbackFunc){
        this._callbackFuncTimaData = callbackFunc;
    }
    getTimeData(TimeData){
//        console.log("-- vmapp : getTimeData --")
        this._callbackFuncTimaData(TimeData);
    }
    // -- センサ情報コールバック設定 --
    setSensorDataCallback(callbackFunc){
        console.log("-- vmapp : setSensorDataCallback : Callback Setting --")
        this._callbackFuncSensorData = callbackFunc;
    }
    getSensorData(sensorData){
//        console.log("-- vfdControllerUI.getSensorData --");
//        console.log(sensorData);
/*        let data_x1 = JSON.parse(sensorData)["val1"];
        let data_x2 = JSON.parse(sensorData)["val2"];
        document.getElementById("temp").innerHTML = data_x1;
        document.getElementById("humidity").innerHTML = data_x2;
*/
        this._callbackFuncSensorData(sensorData);
    }

    //  --CallbackWebsocket送信
    submitWebsocket(data){
      console.log("submitWebsocket:"+data);
      return this.model.websocketSend(data);
    }

    // -- タイトル取得 --
    getTitle(){
        return this.model.getSettingJsonItem("title");
    }

    // -- ネットワーク情報取得 --
    getNetworkSetting(item){
        return this.model.getSettingJsonItem(item);
    }

    // -- 表示設定情報取得 --
    getDisplaySetting(item, ...[num]){
        return this.model.getSettingJsonItem(item,num);
    }
    setDisplaySetting(item, data, ...[num]){
        return this.model.setSettingJsonItem(item, data, num);
    }

/*    getDisplaySetting(item){
        return this.model.getSettingJsonItem(item);
    }
*/
    // -- センサ表示フォーマットデータ設定 --
    getSensorFormat(){
//        let sensorFormat = this.model.getJsonItem("sensorFormat");
        let sensorFormat = [    {"name":"温度","index": -1,"toPrecision": 3 },{"name":"湿度","index": -1,"toPrecision": 3 },{"name":"気圧","index": 0 },
                                {"name":"name4","index": 0 },{"name":"name5","index": 0 },{"name":"name6","index": 0 },{"name":"name7","index": 0 },{"name":"name8","index": 0 } ]

        for(let i=0; i<sensorFormat.length; i++){
            sensorFormat[i].data = "sensor" + i ;
        }
        return sensorFormat
    }

    // -- 時計設定情報取得 --
    getClockSetting(item){
        return this.model.getSettingJsonItem(item);
    }
/*
    getBrightDig(item){
        return this.model.getBrightDig(item);
    }
*/
    getLastUpdateTime(){
        let getLastUpdateTime
        getLastUpdateTime = this.model.getSettingJsonItem("lastUpdateYear").toString() + "/"
        getLastUpdateTime += this.model.getSettingJsonItem("lastUpdateMonth").toString().padStart(2, "0") + "/"
        getLastUpdateTime += this.model.getSettingJsonItem("lastUpdateDay").toString().padStart(2, "0") + " "
        getLastUpdateTime += this.model.getSettingJsonItem("lastUpdateHour").toString().padStart(2, "0") + ":"
        getLastUpdateTime += this.model.getSettingJsonItem("lastUpdateMin").toString().padStart(2, "0")

        return getLastUpdateTime
    }
    // 時刻設定:SNTP設定:SNTP 使用情報送信
    timeConfNtpUsesubmit(data) {
      console.log("timeConfNtpUsesubmit()");
      let jsondat
      if(data == true){
        jsondat = 1;
      }
      else{
        jsondat = 0;
      }
      this.model.websocketSend(this.makeWebsocketData("ntpSet",jsondat));
      return;
    }

    ntpdiffSubmit(hour, min){
        console.log("ntpdiffSubmit()")
        let sendData
        sendData = "{\"ntpDiffHour\" : \"" + hour + "\",\"ntpDiffMin\" : \"" + min + "\"}"
        console.log(sendData)
        this.model.jsonPost(sendData)
    }
    ntpAutoUpdateSubmit(hour, min){
        console.log("ntpAutoUpdateSubmit()")
        let sendData
        sendData = "{\"ntpAutoUpdateHour\" : \"" + hour + "\",\"ntpAutoUpdateMin\" : \"" + min + "\"}"
        console.log(sendData)
        this.model.jsonPost(sendData)
    }

    // 表示設定データ値設定
    submitDisplaySetting(item, data, ...[num]){
      let ret;
      console.log("submitDisplaySetting:"+data+":"+item+":"+num);
      if(typeof num === "number"){
        ret = 0;
      }else{
        ret = this.model.setSettingJsonItem(item, data);
        this.model.websocketSend(this.makeWebsocketData(item,data));
      }
      return ret;
    }

    // tab切替情報送信
    submitChangeTab(data){
        console.log("submitChangeTab:"+data);
        this.model.websocketSend(this.makeWebsocketData("tabName",data));
    }

    // --  WebSocket送信データ作成 --
    makeWebsocketData(item, data){
        let sendData
        sendData = "{\"" + item + "\" : \"" + data + "\"}"
        console.log("sendData:")
        console.log(sendData)
       return(sendData)
    }

    dispBrDigSubmit(num,data){
      console.log("dispBrDigSubmit");
      var ret1,ret2;
  //    this.model.setBrdighitDigtmp(num,data);
      ret1 = this.model.setSettingJsonItem("brDigtmp", data, num)
      ret2 = this.model.websocketSend(this.makeBrightData(num,data));
      if(ret2 != 1){  // websocket not send
 //       ret1 = "";
      }
      return ret1;
    }
    makeBrightData(item, data){
        let sendData
        sendData = "{\"brDig\" : ["
        for(let count=1; count<9+1; count++){
            sendData = sendData + this.model.getSettingJsonItem("brDigtmp", count);
//            sendData = sendData + this.model.getBrightDigtmp(count);
            if(count != 9){
                sendData = sendData + ",";
            }
        }
        sendData = sendData + "]}"
//        console.log(sendData)
    
        return(sendData)
    }
    // 
    resetBrSetting(){
        console.log("vmapp.resetBrSetting");
        this.model.resetBrSetting();
        this.model.websocketSend(this.makeWebsocketData("resetBrSetting",1));
        return;
    }
    // 
    writeBrSetting(){
        console.log("vmapp.writeBrSetting");
        this.model.writeBrSetting();
        this.model.websocketSend(this.makeWebsocketData("writeBrSetting",1));
        return;
    }
/*
    NTPに接続して時刻を取得する
*/
    ntpConnect(){
        console.log("ntpConnect()");
        let sendData
        sendData = "{\"ntpConnect\" : 1}"
        this.model.jsonPost(sendData)

        return;
    }
/*
    RTC設定の送信データを作成
*/
    rtcDataSet(rtcSetDataYear,rtcSetDataMon,rtcSetDataDay,rtcSetDataHour,rtcSetDataMin,rtcSetDataSec){
        console.log("rtcDataSet()");
        let sendData

        sendData = "{\"rtcSetYear\" : \"" + rtcSetDataYear + "\","
        sendData = sendData + "\"rtcSetMon\" : \"" + rtcSetDataMon + "\","
        sendData = sendData + "\"rtcSetDay\" : \"" + rtcSetDataDay + "\","
        sendData = sendData + "\"rtcSetHour\" : \"" + rtcSetDataHour + "\","
        sendData = sendData + "\"rtcSetMin\" : \"" + rtcSetDataMin + "\","
        sendData = sendData + "\"rtcSetSec\" : \"" + rtcSetDataSec + "\"}"

        console.log(sendData)
        this.model.jsonPost(sendData)

        return;
    }
/*
  WiFiアクセスポイントのリストを取得する
*/
    getWifiStaList(){
      console.log("getWifiStaList()");
//      this.model.jsonPost(this.makeWebsocketData("getWifiStaList",1))
      this.model.websocketSend(this.makeWebsocketData("getWifiStaList",1));

      return;
    }

    // SSID設定情報を送信する
    postSsidSetting(ssid,ssidPassword){
      console.log("-- postSsidSetting()");
      let sendData = "{\"ssid\" : \"" + ssid + "\","
      sendData = sendData + "\"ssidPassword\" : \"" + ssidPassword + "\"}"
      console.log(sendData);
      this.model.jsonPost(sendData);
    }
}
