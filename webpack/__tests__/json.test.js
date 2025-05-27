
import { vfdControllerUI } from '../src/viewmodel/vmapp.js';

jest.mock('../src/viewmodel/vmapp.js', () => {
  const originalModule = jest.requireActual('../src/viewmodel/vmapp.js');
  return {
    __esModule: true,
    ...originalModule,
    vfdControllerUI: jest.fn().mockImplementation(function(initialSetting) {
      const originalInstance = new originalModule.vfdControllerUI(initialSetting);
      originalInstance.model.websocketSend = jest.fn().mockReturnValue('Mocked websocketSend');
      originalInstance.getTitle = jest.fn().mockReturnValue('Mocked method 1');
      return originalInstance;
    })
  };
});

describe('vfdControllerUI', () => {
  it('should use mocked websocketSend when calling submitWebsocket', () => {
    const vmUI = new vfdControllerUI('{"title":"VFD Clock Controler"}');
    expect(vmUI.submitWebsocket()).toEqual('Mocked websocketSend');
    expect(vmUI.getTitle()).toEqual('Mocked method 1');

    expect(vmUI.model.setSettingJsonItem("name","Takahiro")).toMatch("Takahiro");
    expect(vmUI.model.setSettingJsonItem("brDig",98,2)).toBe(98);

    // setSettingJsonItem refactoring Test
    expect(vmUI.setDisplaySetting("test",15)).toBe(15);
    expect(vmUI.setDisplaySetting("brDig",12,1)).toBe(12);

    expect(vmUI.submitDisplaySetting("test2", 12)).toBe(12);

    expect(vmUI.dispBrDigSubmit(1,67)).toBe(67);

 
  });
});
