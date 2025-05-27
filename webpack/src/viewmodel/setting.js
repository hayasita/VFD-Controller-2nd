/*
  Webインタフェースのリソース情報の読み込みを行う。
  
*/

import i18next from 'i18next';
//import {langData} from '../infra/vfdi18n.js'
import ModelSelectionLocaleEn from '../locales/en/menu.json'
import ModelSelectionLocaleJa from '../locales/ja/menu.json'
import ModelSelectionTimeZone from '../infra/timezone.json'
import tzLocaleEn from '../locales/en/tzlang.json'
import tzLocaleJa from '../locales/ja/tzlang.json'
import ModelSelectionTimeZoneArea from '../infra/timezoneArea.json'
//import timezoneList from '../infra/timezone.js'

export class ModelselectionSetting {

  constructor(model) {
    console.log("== Setting.js ==");
    this.model = model;
//    this.event = event;

    // 初期値設定
    let initValue = this._getInitialSettings();

    // LocaleId初期値取得
    let localesIndex = this.model.getSettingJsonItem("localesId");
    if(localesIndex == 0){
      this.model.localesId = "ja";
    }
    else if(localesIndex == 1){
      this.model.localesId = "en";
    }
    else{
      this.model.localesId = initValue.localesId;     // this.model.localesId が無効値の場合
    }

    // TimeZone 設定
    this.model.timezone = ModelSelectionTimeZone;
//    console.log(this.model.timezone);

    this.model.timezoneArea = ModelSelectionTimeZoneArea;
//    console.log(this.model.timezoneArea);

//    model.setLanguage(this.localeID);
//    langData.lng = this.localeID;
//    langData.resources.en.translation = enTranslation;
//    langData.resources.jp.translation = jpTranslation;
    Object.assign(ModelSelectionLocaleEn, tzLocaleEn);
    Object.assign(ModelSelectionLocaleJa, tzLocaleJa);
    let rs = {en:{translation:ModelSelectionLocaleEn},ja:{translation:ModelSelectionLocaleJa}}
    //    i18next.init(langData);
    i18next.init({
    //  lng: this.localeID,
      lng: this.model.localesId,
      fallbackLng: 'ja',
      debug: false,
      resources: rs
    })
  //  i18next.changeLanguage('ja');
  //  i18next.changeLanguage(this.model.localesId);
    this.setLanguage(this.model.localesId);

    this.model.eventName = {
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

  }

  // -- 言語設定 --
  setLanguage(localeID){
//      this.localeID = localeID;
      i18next.changeLanguage(localeID);
  }
  // -- 設定言語取得 --
//  getLanguage(){
//      return this.localeID;
//  }

  _getInitialSettings() {
    let initValue = {};
    initValue.localesId = 'ja';
    return initValue;
  }
}

