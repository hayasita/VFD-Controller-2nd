
import { _initial_setting_ } from'../src_test/setting.js'; // パスは実際の設定に合わせる
import { vfdControllerDomain } from '../src/model/vfdControllerDomain';
import {ModelselectionSetting} from '../src/viewmodel/setting'

//console.log(this);
//console.log(_initial_setting_);
describe('vfdControllerDomain', () => {

  test('model eventString', () => {
    const model = new vfdControllerDomain(_initial_setting_);
    const setting = new ModelselectionSetting(model);

    let eventLogName = {
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

    expect(model.eventName.size).toBe(eventLogName.size);
    for(const key in eventLogName){
      expect(eventLogName[key]).toBe(model.eventName[key]);
    }

//    expect(model.eventName[0x10]).toBe(eventLogName[0x10]);
  });
});


