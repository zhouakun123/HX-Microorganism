unit main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, Grids, DBGrids, Menus, StdCtrls, DB, ADODB, ExtCtrls, ComObj;

const
  // Command define
  UPLOAD_FREQ_UP = $80;
  // Operation code define
  NULL = 0;                         // NULL
  CLR_ALL = 1;
  // Time define
  T_100MS = 2;                      // Unit 50ms
  T_1S = 20;                        // Unit 50ms
  T_2S = 40;                        // Unit 50ms
  T_3S = 60;                        // Unit 50ms  
  T_5S = 100;                       // Unit 50ms

type
  TForm_main = class(TForm)
    DBGrid1: TDBGrid;
    PopupMenu1: TPopupMenu;
    N2: TMenuItem;
    ADOConnection1: TADOConnection;
    ADOQuery1: TADOQuery;
    DataSource1: TDataSource;
    Timer2: TTimer;
    ADOQuery2: TADOQuery;
    N1: TMenuItem;
    SaveDialog1: TSaveDialog;
    Timer1: TTimer;
    procedure ExportDBGrid;
    procedure OpenDatabase;
    procedure CloseDatabase;
    procedure FormCreate(Sender: TObject);
    procedure FormClose(Sender: TObject; var Action: TCloseAction);
    procedure FormShow(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);
    procedure FormResize(Sender: TObject);
    procedure Timer2Timer(Sender: TObject);
    procedure N1Click(Sender: TObject);
    procedure N2Click(Sender: TObject);
  private
    id: Integer;
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form_main: TForm_main;
  sendBuffer: array of byte;                  // Send buffer
  recvTimeCnt: word;
  gDateStr,gTimeStr: string;
  gIndex: Integer;

implementation

uses SerialSet,common;

{$R *.dfm}

// 连接数据库
procedure TForm_main.OpenDatabase;
var
  Path: string;
begin
  Path := ExtractFilePath(Application.ExeName);
  ADOConnection1.ConnectionString := 'Provider=Microsoft.Jet.OLEDB.4.0;Data Source= ' + Path + 'Data.mdb;Persist Security Info=False';    //设置数据连接
  ADOConnection1.Close;
  ADOConnection1.Open;        
end;

// 关闭数据库
procedure TForm_main.CloseDatabase;
begin
  ADOConnection1.Close;
end;

procedure TForm_main.FormCreate(Sender: TObject);
begin
  OpenDatabase;                                   // 连接数据库
  //FormStyle := fsStayOnTop;
  WindowState := wsMaximized;                     // 默认设置最大化
  GenPoly8 := $07;
  SetLength(sendBuffer, 20);
  Refresh;
end;

procedure TForm_main.FormClose(Sender: TObject; var Action: TCloseAction);
begin
  CloseDatabase;
  Application.Terminate;
end;

procedure TForm_main.FormShow(Sender: TObject);
var
  dbgridwidth: Double;
begin
  dbgridwidth := DBGrid1.Width/7;
  DBGrid1.Columns[1].Width := Trunc(dbgridwidth)-5;
  DBGrid1.Columns[2].Width := Trunc(dbgridwidth)-5;
  DBGrid1.Columns[3].Width := Trunc(dbgridwidth)-5;
  DBGrid1.Columns[4].Width := Trunc(dbgridwidth)-5;
  DBGrid1.Columns[5].Width := Trunc(dbgridwidth)-5;
  DBGrid1.Columns[6].Width := Trunc(dbgridwidth)-5;
  DBGrid1.Columns[7].Width := Trunc(dbgridwidth)-5;
  DBGrid1.Columns[1].Alignment := taCenter;
  DBGrid1.Columns[2].Alignment := taCenter;
  DBGrid1.Columns[3].Alignment := taCenter;
  DBGrid1.Columns[4].Alignment := taCenter;
  DBGrid1.Columns[5].Alignment := taCenter;
  DBGrid1.Columns[6].Alignment := taCenter;
  DBGrid1.Columns[7].Alignment := taCenter;
  DBGrid1.Columns[1].Title.Alignment := taCenter;
  DBGrid1.Columns[2].Title.Alignment := taCenter;
  DBGrid1.Columns[3].Title.Alignment := taCenter;
  DBGrid1.Columns[4].Title.Alignment := taCenter;
  DBGrid1.Columns[5].Title.Alignment := taCenter;
  DBGrid1.Columns[6].Title.Alignment := taCenter;
  DBGrid1.Columns[7].Title.Alignment := taCenter;
  DBGrid1.Columns[1].Title.Caption := '颜色点';
  DBGrid1.Columns[2].Title.Caption := 'Red频率';
  DBGrid1.Columns[3].Title.Caption := 'Blue频率';
  DBGrid1.Columns[4].Title.Caption := 'Clear频率';
  DBGrid1.Columns[5].Title.Caption := 'Green频率';
  DBGrid1.Columns[6].Title.Caption := '日期';
  DBGrid1.Columns[7].Title.Caption := '时间';
  Timer1.Enabled := True;                           // 使能定时器调整窗体大小
end;

procedure TForm_main.Timer1Timer(Sender: TObject);
begin
  Timer1.Enabled := False;
  Timer2.Enabled := True;
  WindowState := wsNormal;                          // 设置窗体正常状态,下面设置的窗体为正常大小
  ClientWidth := (Screen.Width div 2) + 200;        // 设置窗体显示区域大小
  ClientHeight := (Screen.Height div 2) + 90;
  Top := (Screen.Height - ClientHeight) div 2;      // 设置显示位置,显示在屏幕中间
  if Top > 60 then Top := Top - 60;
  Left := (Screen.Width - ClientWidth) div 2;
  Refresh_DIS(ADOQuery1,'select * from freq_info order by dot desc, dates desc, times desc');
  Form_SerialSet.OpenPort();                        // 每次打开软件,默认打开串口
end;

procedure TForm_main.FormResize(Sender: TObject);
var
  dbgridwidth: Double;
begin
  dbgridwidth := DBGrid1.Width/7;
  DBGrid1.Columns[1].Width := Trunc(dbgridwidth)-5;
  DBGrid1.Columns[2].Width := Trunc(dbgridwidth)-5;
  DBGrid1.Columns[3].Width := Trunc(dbgridwidth)-5;
  DBGrid1.Columns[4].Width := Trunc(dbgridwidth)-5;
  DBGrid1.Columns[5].Width := Trunc(dbgridwidth)-5;
  DBGrid1.Columns[6].Width := Trunc(dbgridwidth)-5;
  DBGrid1.Columns[7].Width := Trunc(dbgridwidth)-5;
end;

procedure TForm_main.Timer2Timer(Sender: TObject);
var
  i,len: word;
  cmd,temp,crc: Byte;
  dataBuf: array[0..5000] of byte;
  tempBuf: array[0..5000] of byte;
  freq1Str,freq2Str,freq3Str,freq4Str: string;
  dateStr,timeStr: string;
begin

  recvTimeCnt := recvTimeCnt + 1;
  if recvTimeCnt >= T_100MS then
  begin
    recvTimeCnt := 0;
    len := Length(recvBuffer);
    if len <> 0 then
    begin
      tempBuf[0] := byte(recvBuffer[1]);                    // Char to HEX, Save head bytes
      tempBuf[1] := byte(recvBuffer[2]);                    // Char to HEX, Save command byte
      tempBuf[2] := byte(recvBuffer[3]);                    // Char to HEX, Save status byte
      tempBuf[3] := byte(recvBuffer[4]);                    // Char to HEX, Save length high byte
      tempBuf[4] := byte(recvBuffer[5]);                    // Char to HEX, Save length low byte
      if tempBuf[0] = $FF then
      begin
        len := tempBuf[4];
        len := len or (tempBuf[3] shl 8);
        if len > 0 then
        begin
          tempBuf[len+1] := byte(recvBuffer[len+1+1]);
          if tempBuf[len+1] = $0D then
          begin
            for i := 4 to len do
            tempBuf[i] := byte(recvBuffer[i+1]);                  // Char to HEX
            recvBuffer := '';

            crc := 0;                                             // CRC初始值
            for i := 0 to len - 1 do
            begin
            temp := tempBuf[i];
            crc := CalCRC8(temp,crc,GenPoly8);                    // 按位计算CRC
            end;

            if crc = tempBuf[len] then
            begin
              cmd := tempBuf[1];
              if cmd = UPLOAD_FREQ_UP then
              begin
                fillchar(dataBuf,sizeof(dataBuf),0);
                CopyMemory(@dataBuf[0], @tempBuf[5], len - 4);
                DateStr := formatdatetime('yyyy-mm-dd', now());   // 获取系统日期
                TimeStr := formatdatetime('hh:mm:ss', now());     // 获取系统时间
                for i := 0 to 72 - 1 do
                begin
                freq1Str := IntToStr(tempBuf[i*12+5] or (tempBuf[i*12+6] shl 8) or (tempBuf[i*12+7] shl 16));
                freq2Str := IntToStr(tempBuf[i*12+8] or (tempBuf[i*12+9] shl 8) or (tempBuf[i*12+10] shl 16));
                freq3Str := IntToStr(tempBuf[i*12+11] or (tempBuf[i*12+12] shl 8) or (tempBuf[i*12+13] shl 16));
                freq4Str := IntToStr(tempBuf[i*12+14] or (tempBuf[i*12+15] shl 8) or (tempBuf[i*12+16] shl 16));
                // 在insert语句里如果字段写入的内容的是中文汉字,要加quotedstr
                Write_SQL(ADOQuery2,'insert into freq_info([dot],[freq1],[freq2],[freq3],[freq4],[dates],[times]) values ('+IntToStr(i+1)+','+QuotedStr(freq1Str)+','+QuotedStr(freq2Str)+','+QuotedStr(freq3Str)+','+QuotedStr(freq4Str)+','+QuotedStr(DateStr)+','+QuotedStr(TimeStr)+')');
                end;
                Refresh_DIS(ADOQuery1,'select * from freq_info order by dot desc, dates desc, times desc');
              end;
            end;
          end
          else recvBuffer := '';
        end
        else recvBuffer := '';
      end
      else recvBuffer := '';
    end;
  end;
end;

procedure TForm_main.N2Click(Sender: TObject);
begin
  Write_SQL(ADOQuery2,'delete * from freq_info');           // 删除表全部记录
  Refresh_DIS(ADOQuery1,'select * from freq_info');         // 更新显示
end;

procedure TForm_main.N1Click(Sender: TObject);
begin
    ExportDBGrid();
end;

procedure TForm_main.ExportDBGrid();
var
  xlsFileName: string;
  eclApp,WorkBook: Variant;
  iRowNow,iRowAll,iColNow,iColAll: integer;
begin
  if DBGrid1.DataSource.DataSet.RecordCount <= 0 then
  begin
    showmessage('数据为空!');
    exit;
  end;
  xlsFileName := 'c:\freq.xlsx';
  try  // 创建excel；
    eclApp := CreateOleObject('Excel.Application');
    WorkBook := CreateOleObject('Excel.sheet');
  except
    Application.Terminate;
  end;
  try
    WorkBook := eclApp.workbooks.add;
    iColAll := DBGrid1.Columns.Count;
    iRowAll := DBGrid1.DataSource.DataSet.RecordCount;
    iColNow := 2;
    while iColNow <= iColAll do // 处理一行的不同列
    begin
      eclApp.cells(1,iColNow-1) := DBGrid1.Columns.Items[iColNow-1].Title.caption;
      Inc(iColNow);
    end;
    iRowNow := 2; // 第一行留做标题
    begin // 按格导入数据
      DBGrid1.DataSource.DataSet.First;
      while iRowNow <= iRowAll + 1 do
      begin
//                    DBGrid1.DataSource.DataSet.Fields[];
        iColNow := 2;
        while iColNow <= iColAll do // 处理一行的不同列
        begin
          eclApp.cells(iRowNow,iColNow-1) := DBGrid1.Fields[iColNow-1].Text;
          Inc(iColNow);
        end;
        DBGrid1.DataSource.DataSet.Next;
        Inc(iRowNow);
      end;
    end;// 按格导入数据
// 测试
//        eclApp.cells(1,1):='test';
    SaveDialog1.InitialDir := ExtractFilePath(Application.ExeName);
    if SaveDialog1.Execute then
    begin
      xlsFileName := SaveDialog1.FileName + '.xlsx';
      ShowMessage('数据导出成功！'#13#10'导出文件为：' + xlsFileName);
      WorkBook.saveas(xlsFileName);
      WorkBook.close;
      eclApp.quit;
//    end
//    else
//    begin
//    ShowMessage('系统出错，请重新打开系统！');
    end;
  except
  ShowMessage('请检查是否正确安装MicroSoft Excel!');
  end;
end;

end.
