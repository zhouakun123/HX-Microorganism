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
  GenPoly8: byte;                                       // CRC-8 = x8+x2+x+1, 00000111($07) ��λ����
implementation

{-------------------------------------------------------------------------------
  ������:    CalCRC8
  ����:      8λCRC����λ���㣬�ٶ�������ռ�ÿռ�����
  ����:      data - ��У������; crc - ǰһ��У����; genpoly - У�鳣�� 
  ����ֵ:    У����
  ��ע:      �������Ǹ�λ����
-------------------------------------------------------------------------------}
function CalCRC8(data, crc, genpoly: Byte): Byte;
var i: Integer;
begin
  crc := crc xor data;
  for i:=0 to 7 do
    if (crc and $80) <> 0 then                          // ֻ�������λ
      crc := (crc shl 1) xor genpoly                    // ���λΪ1����λ�������
    else crc := crc shl 1;                              // ����ֻ��λ
  Result := crc;
end;

{-------------------------------------------------------------------------------
  ������:    Delay
  ����:      �ӳ�N ms
  ����:      dwMilliseconds - �ӳٵ�ms��
  ����ֵ:    NONE
  ��ע:
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
  ������:    Write_SQL
  ����:      ���Ӽ�¼
  ����:      ADOQueryTemp : TADOQuery;SQLString: string
  ����ֵ:    NONE
  ��ע:
-------------------------------------------------------------------------------}
procedure Write_SQL (ADOQueryTemp : TADOQuery;SQLString: string);
begin
  ADOQueryTemp.Close;
  ADOQueryTemp.SQL.Clear;
  aDOQueryTemp.SQL.Add(SQLString);
  ADOQueryTemp.ExecSQL;
end;

{-------------------------------------------------------------------------------
  ������:    Serch_SQL
  ����:      ��ѯ��¼
  ����:      ADOQueryTemp : TADOQuery;SQLString: string
  ����ֵ:    NONE
  ��ע:
-------------------------------------------------------------------------------}
procedure Serch_SQL (ADOQueryTemp : TADOQuery;SQLString: string);
begin
  ADOQueryTemp.Close;
  ADOQueryTemp.SQL.Clear;
  ADOQueryTemp.SQL.Add(SQLString);
  ADOQueryTemp.Open;
end;

{-------------------------------------------------------------------------------
  ������:    Refresh_DIS
  ����:      ������ʾ 
  ����:      ADOQueryTemp : TADOQuery;SQLString: string
  ����ֵ:    NONE
  ��ע:
-------------------------------------------------------------------------------}
procedure Refresh_DIS (ADOQueryTemp : TADOQuery;SQLString: string);
begin
  ADOQueryTemp.Close;
  ADOQueryTemp.SQL.Text:=SQLString;
  ADOQueryTemp.Open;
end;

end.
