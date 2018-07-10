unit SerialSet;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, ExtCtrls, StdCtrls, Spcomm, DB, ADODB;

type
  TForm_SerialSet = class(TForm)
  GroupBox1: TGroupBox;
  Button_Ok: TButton;
  Button_Quit: TButton;
  Comm1: TComm;
  ComboBox_Num: TComboBox;
  ComboBox_Bps: TComboBox;
  Label_Num: TLabel;
  Label_Bps: TLabel;
    ADOQuery1: TADOQuery;
  procedure OpenPort();
  procedure FormCreate(Sender: TObject);
  procedure Button_QuitClick(Sender: TObject);
  procedure Comm1ReceiveData(Sender: TObject; Buffer: Pointer; BufferLength: Word);
    procedure Button_OkClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form_SerialSet: TForm_SerialSet;
  serialSta: Boolean;
  recvBuffer: string;

implementation
uses
  main, common;
{$R *.dfm}
              
// 获取电脑串口列表
procedure EnumComPorts(Ports: TStrings);
var
  KeyHandle: HKEY;
  ErrCode, Index: Integer;
  ValueName, Data: string;
  ValueLen, DataLen, ValueType: DWORD;
  TmpPorts: TStringList;
begin
  ErrCode := RegOpenKeyEx(
    HKEY_LOCAL_MACHINE,
    'HARDWARE\DEVICEMAP\SERIALCOMM',
    0,
    KEY_READ,
    KeyHandle);

  if ErrCode <> ERROR_SUCCESS then
    Exit;  // raise EComPort.Create(CError_RegError, ErrCode);

  TmpPorts := TStringList.Create;
  try
    Index := 0;
    repeat
      ValueLen := 256;
      DataLen := 256;
      SetLength(ValueName, ValueLen);
      SetLength(Data, DataLen);
      ErrCode := RegEnumValue(
        KeyHandle,
        Index,
        PChar(ValueName),
        Cardinal(ValueLen),
        nil,
        @ValueType,
        PByte(PChar(Data)),
        @DataLen);

      if ErrCode = ERROR_SUCCESS then
      begin
        SetLength(Data, DataLen);
        TmpPorts.Add(Data);
        Inc(Index);
      end
      else
        if ErrCode <> ERROR_NO_MORE_ITEMS then
          exit; //raise EComPort.Create(CError_RegError, ErrCode);

    until (ErrCode <> ERROR_SUCCESS) ;

    TmpPorts.Sort;
    Ports.Assign(TmpPorts);
  finally
    RegCloseKey(KeyHandle);
    TmpPorts.Free;
  end;
end;

// 打开串口
procedure TForm_SerialSet.OpenPort();
begin
  if serialSta = True then                          // 串口打开了吗
  begin
  serialSta := False;
  Comm1.StopComm;                                   // 首先关闭串口
  end;
  Comm1.CommName := '//./' + ComboBox_Num.Items[ComboBox_Num.ItemIndex];
  Comm1.Inx_XonXoffFlow := False;
  Comm1.Outx_XonXoffFlow := False;
  Comm1.ReadIntervalTimeout := 8;                   // 设置字节间的超时时间8ms
  Comm1.ReadTotalTimeoutMultiplier := 0;
  Comm1.ReadTotalTimeoutConstant := 0;
  //Comm1.WriteTotalTimeoutMultiplier := 50;
  //Comm1.WriteTotalTimeoutConstant := 2000;
  Serch_SQL(ADOQuery1,'select bpsIndex from soft_set');
  if ADOQuery1.RecordCount > 0 then                // 之前设置过吗
  ComboBox_Bps.ItemIndex := ADOQuery1.fieldbyname('bpsIndex').asinteger;
  //Comm1.BaudRate := StrToInt(ComboBox_Bps.Items[ComboBox_Bps.ItemIndex]);
  Comm1.BaudRate := 19200;
  Comm1.Parity := None;
  Comm1.ByteSize := _8;
  Comm1.StopBits := _1;
  Comm1.StartComm;
  serialSta := True;
end;

procedure TForm_SerialSet.FormCreate(Sender: TObject);
var
  mystrings: tstrings;
  mystr: string;
begin
  mystrings := tstringlist.Create;                  // 创建实例 
  EnumComPorts(mystrings);                          // 得到串口列表
  mystr := mystrings.Text;                          // 这个是读取所有内容,每行中间有换行符分割
  mystrings.Free;                                   // 释放
  ComboBox_Bps.ItemIndex := 0;
  ComboBox_Num.ItemIndex := 0;
  if Pos('COM40',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 39;
  end else
  if Pos('COM39',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 38;
  end else
  if Pos('COM38',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 37;
  end else
  if Pos('COM37',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 36;
  end else
  if Pos('COM36',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 35;
  end else
  if Pos('COM35',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 34;
  end else
  if Pos('COM34',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 33;
  end else
  if Pos('COM33',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 32;
  end else
  if Pos('COM32',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 31;
  end else
  if Pos('COM31',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 30;
  end else
  if Pos('COM30',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 29;
  end else
  if Pos('COM29',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 28;
  end else
  if Pos('COM28',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 27;
  end else
  if Pos('COM27',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 26;
  end else
  if Pos('COM26',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 25;
  end else
  if Pos('COM25',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 24;
  end else
  if Pos('COM24',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 23;
  end else
  if Pos('COM23',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 22;
  end else
  if Pos('COM22',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 21;
  end else
  if Pos('COM21',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 20;
  end else
  if Pos('COM20',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 19;
  end else
  if Pos('COM19',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 18;
  end else
  if Pos('COM18',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 17;
  end else
  if Pos('COM17',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 16;
  end else
  if Pos('COM16',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 15;
  end else
  if Pos('COM15',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 14;
  end else
  if Pos('COM14',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 13;
  end else
  if Pos('COM13',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 12;
  end else
  if Pos('COM12',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 11;
  end else
  if Pos('COM11',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 10;
  end else
  if Pos('COM10',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 9;
  end else
  if Pos('COM9',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 8;
  end else
  if Pos('COM8',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 7;
  end else
  if Pos('COM7',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 6;
  end else
  if Pos('COM6',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 5;
  end else
  if Pos('COM5',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 4;
  end else
  if Pos('COM4',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 3;
  end else
  if Pos('COM3',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 2;
  end else
  if Pos('COM2',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 1;
  end else
  if Pos('COM1',mystr) > 0 then
  begin
  ComboBox_Num.ItemIndex := 0;
  end;
end;

procedure TForm_SerialSet.Comm1ReceiveData(Sender: TObject; Buffer: Pointer;
  BufferLength: Word);
var
  str :string;
  len :Word;
begin
  SetLength(str,BufferLength);
  move(buffer^,pchar(@str[1])^,bufferlength);       // Serial Receive Data
  recvBuffer := recvBuffer + str;
  len := Length(recvBuffer);
  if len > 1000 then
  recvBuffer := '';                                 // 接收超过1000清空
  recvTimeCnt := 0;
end;

procedure TForm_SerialSet.Button_QuitClick(Sender: TObject);
begin
  Close;
end;

procedure TForm_SerialSet.Button_OkClick(Sender: TObject);
begin
  if serialSta = True then                          // 串口打开了吗
  begin
  serialSta := False;
  Comm1.StopComm;                                   // 首先关闭串口
  end;
  Comm1.CommName := '//./' + ComboBox_Num.Items[ComboBox_Num.ItemIndex];
  Comm1.Inx_XonXoffFlow := False;
  Comm1.Outx_XonXoffFlow := False;
  Comm1.ReadIntervalTimeout := 8;                   // 设置字节间的超时时间8ms
  Comm1.ReadTotalTimeoutMultiplier := 0;
  Comm1.ReadTotalTimeoutConstant := 0;
  //Comm1.WriteTotalTimeoutMultiplier := 50;
  //Comm1.WriteTotalTimeoutConstant := 2000;
  Comm1.BaudRate := StrToInt(ComboBox_Bps.Items[ComboBox_Bps.ItemIndex]);
  Comm1.Parity := None;
  Comm1.ByteSize := _8;
  Comm1.StopBits := _1;
  Comm1.StartComm;
  serialSta := True;
  Serch_SQL(ADOQuery1,'select bpsIndex from soft_set');
  if ADOQuery1.RecordCount > 0 then                 // 之前设置过吗
  begin
  Write_SQL(ADOQuery1,'UPDATE soft_set SET bpsIndex='''+IntToStr(ComboBox_Bps.ItemIndex)+'''');     // 默认修改第一行
  end else
  Write_SQL(ADOQuery1,'insert into soft_set([bpsIndex]) values ('+QuotedStr(IntToStr(ComboBox_Bps.ItemIndex))+')');  // 增加新记录
  Close;
end;

end.
