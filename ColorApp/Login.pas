unit Login;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, WinSkinData, DB, ADODB;

type
  TForm_Login = class(TForm)
    Label1: TLabel;
    Label2: TLabel;
    ComboBox_name: TComboBox;
    Edit_password: TEdit;
    Button_Login: TButton;
    Button_Quit: TButton;
    ADOQuery1: TADOQuery;
    procedure Button_LoginClick(Sender: TObject);
    procedure Button_QuitClick(Sender: TObject);
    procedure FromShow(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form_Login: TForm_Login;
  loginTimes: Integer;
  loginName: string;          // �����½���û���
  loginRoot: string;          // �����½�û���Ȩ��

implementation
uses
  main,common;

{$R *.dfm}

procedure TForm_Login.Button_LoginClick(Sender: TObject);
var
 lastTime,userName,userPass:string;       
begin
  inc(LoginTimes);
  if LoginTimes > 3 then                  // �����½���� logintimes���������������ʱ
  begin
    showmessage('���Ѿ������������֤����,���˳���¼��');
    Form_main.CloseDatabase;              // �ر����ݿ�
    application.Terminate;                // ��ֹӦ�ó���ִ��
  end
  else
  begin
    if (length(trim(ComboBox_name.Text)) > 0) or (length(trim(Edit_password.Text)) > 0) then
    begin
      userName := trim(ComboBox_name.Text);           //ȡ���û�������û���
      userPass := trim(Edit_password.Text);           //ȡ���û����������
      Serch_SQL(ADOQuery1,'select * from login_info where (username='''+userName+''') and(userpass='''+UserPass+''')');
      if ADOQuery1.RecordCount > 0 then             //����õ��ļ�¼����0����˵���ҵ����������ļ�¼,�û�����������ȷ
      begin                                         //��֤ͨ����,�����û��ĵ�½ʱ��͵�½����
        loginRoot := ADOQuery1.fieldbyname('root').asstring;  //��ȡ�û�Ȩ���ֶ�
        loginName := ComboBox_name.Text;            //�����½���û���
        lastTime := DateTimeToSTR(date);
        Write_SQL(ADOQuery1,'update login_info set times = times+1,lasttime='''+lasttime+''' where (username='''+userName+''')');
        Form_Login.Hide;
        Form_main.Show;
        Form_main.Visible := True;
      end
      else
      begin
        showmessage('��ȷ�ϵ�¼���û����������Ƿ���ȷ��');
        ComboBox_name.SetFocus;   // ȡ�����뽹��
        ComboBox_name.SelectAll;  // ѡ���ı����е���������
      end
    end
    else
    showmessage('�û��������벻��Ϊ�գ�');
  end;
end;

procedure TForm_Login.Button_QuitClick(Sender: TObject);
begin
  Close;
end;

// �����ݿ�login���ȡ�û�����Ϣ
procedure TForm_Login.FromShow(Sender: TObject);
var
  tempStr: string;
begin
  Top := (Screen.Height div 2) - (ClientHeight div 2);     // ������ʾλ��,��ʾ����Ļ�м�
  Left := (Screen.Width div 2) - (ClientWidth div 2);
  Serch_SQL(ADOQuery1,'select username from login_info');
  if ADOQuery1.RecordCount > 0 then
  begin
    ADOQuery1.first;
    while( not ADOQuery1.eof) do
    begin
    tempStr := ADOQuery1.fields[0].asstring;    // ��ȡ,����1
    //tempStr := ADOQuery1.fieldbyname('username').asstring;  //��ȡ,����2
    ComboBox_name.Items.Add(tempStr);           // ��ӵ�ComboBox_name.Items
    ADOQuery1.next;
    end;
  end;
  ComboBox_name.ItemIndex := 0;
end;

end.
