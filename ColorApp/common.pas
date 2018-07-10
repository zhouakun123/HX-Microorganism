unit common;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, DB, Grids, DBGrids, ADODB, ExtCtrls, ComCtrls, Buttons,StrUtils,ShellAPI;

  function CalCRC8(data, crc, genpoly: Byte): Byte;  
  procedure Delay(dwMilliseconds: DWORD);
  procedure Write_SQL (ADOQueryTemp : TADOQuery;SQLString: string);
  procedure Serch_SQL (ADOQueryTemp : TADOQuery;SQLString: string);
  procedure Refresh_DIS (ADOQueryTemp : TADOQuery;SQLString: string);

var
  GenPoly8: byte;                                       // CRC-8 = x8+x2+x+1, 00000111($07) 高位先行
implementation

{-------------------------------------------------------------------------------
  函数名:    CalCRC8
  功能:      8位CRC：按位计算，速度最慢，占用空间最少
  参数:      data - 待校验数据; crc - 前一次校验结果; genpoly - 校验常量 
  返回值:    校验结果
  备注:      数据流是高位先行
-------------------------------------------------------------------------------}
function CalCRC8(data, crc, genpoly: Byte): Byte;
var i: Integer;
begin
  crc := crc xor data;
  for i:=0 to 7 do
    if (crc and $80) <> 0 then                          // 只测试最高位
      crc := (crc shl 1) xor genpoly                    // 最高位为1，移位和异或处理
    else crc := crc shl 1;                              // 否则只移位
  Result := crc;
end;

{-------------------------------------------------------------------------------
  过程名:    Delay
  功能:      延迟N ms
  参数:      dwMilliseconds - 延迟的ms数
  返回值:    NONE
  备注:
-------------------------------------------------------------------------------}
procedure Delay(dwMilliseconds: DWORD);
var
iStart,iStop:DWORD;
begin
  iStart := GetTickCount();
  repeat
  iStop  := GetTickCount();
  Application.ProcessMessages;
  until (iStop - iStart) >= dwMilliseconds;
end;

{-------------------------------------------------------------------------------
  过程名:    Write_SQL
  功能:      增加记录
  参数:      ADOQueryTemp : TADOQuery;SQLString: string
  返回值:    NONE
  备注:
-------------------------------------------------------------------------------}
procedure Write_SQL (ADOQueryTemp : TADOQuery;SQLString: string);
begin
  ADOQueryTemp.Close;
  ADOQueryTemp.SQL.Clear;
  aDOQueryTemp.SQL.Add(SQLString);
  ADOQueryTemp.ExecSQL;
end;

{-------------------------------------------------------------------------------
  过程名:    Serch_SQL
  功能:      查询记录
  参数:      ADOQueryTemp : TADOQuery;SQLString: string
  返回值:    NONE
  备注:
-------------------------------------------------------------------------------}
procedure Serch_SQL (ADOQueryTemp : TADOQuery;SQLString: string);
begin
  ADOQueryTemp.Close;
  ADOQueryTemp.SQL.Clear;
  ADOQueryTemp.SQL.Add(SQLString);
  ADOQueryTemp.Open;
end;

{-------------------------------------------------------------------------------
  过程名:    Refresh_DIS
  功能:      更新显示 
  参数:      ADOQueryTemp : TADOQuery;SQLString: string
  返回值:    NONE
  备注:
-------------------------------------------------------------------------------}
procedure Refresh_DIS (ADOQueryTemp : TADOQuery;SQLString: string);
begin
  ADOQueryTemp.Close;
  ADOQueryTemp.SQL.Text:=SQLString;
  ADOQueryTemp.Open;
end;

end.
