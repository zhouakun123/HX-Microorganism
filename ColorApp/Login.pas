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
  loginName: string;          // 保存登陆的用户名
  loginRoot: string;          // 保存登陆用户的权限

implementation
uses
  main,common;

{$R *.dfm}

procedure TForm_Login.Button_LoginClick(Sender: TObject);
var
 lastTime,userName,userPass:string;       
begin
  inc(LoginTimes);
  if LoginTimes > 3 then                  // 如果登陆次数 logintimes大于允许的最大次数时
  begin
    showmessage('你已经超过允许的验证次数,将退出登录！');
    Form_main.CloseDatabase;              // 关闭数据库
    application.Terminate;                // 终止应用程序执行
  end
  else
  begin
    if (length(trim(ComboBox_name.Text)) > 0) or (length(trim(Edit_password.Text)) > 0) then
    begin
      userName := trim(ComboBox_name.Text);           //取得用户输入的用户名
      userPass := trim(Edit_password.Text);           //取得用户输入的密码
      Serch_SQL(ADOQuery1,'select * from login_info where (username='''+userName+''') and(userpass='''+UserPass+''')');
      if ADOQuery1.RecordCount > 0 then             //如果得到的记录大于0条，说明找到满足条件的记录,用户名和密码正确
      begin                                         //验证通过后,更新用户的登陆时间和登陆次数
        loginRoot := ADOQuery1.fieldbyname('root').asstring;  //读取用户权限字段
        loginName := ComboBox_name.Text;            //保存登陆的用户名
        lastTime := DateTimeToSTR(date);
        Write_SQL(ADOQuery1,'update login_info set times = times+1,lasttime='''+lasttime+''' where (username='''+userName+''')');
        Form_Login.Hide;
        Form_main.Show;
        Form_main.Visible := True;
      end
      else
      begin
        showmessage('请确认登录的用户名和密码是否正确！');
        ComboBox_name.SetFocus;   // 取得输入焦点
        ComboBox_name.SelectAll;  // 选择文本框中的所有内容
      end
    end
    else
    showmessage('用户名和密码不能为空！');
  end;
end;

procedure TForm_Login.Button_QuitClick(Sender: TObject);
begin
  Close;
end;

// 从数据库login表读取用户名信息
procedure TForm_Login.FromShow(Sender: TObject);
var
  tempStr: string;
begin
  Top := (Screen.Height div 2) - (ClientHeight div 2);     // 设置显示位置,显示在屏幕中间
  Left := (Screen.Width div 2) - (ClientWidth div 2);
  Serch_SQL(ADOQuery1,'select username from login_info');
  if ADOQuery1.RecordCount > 0 then
  begin
    ADOQuery1.first;
    while( not ADOQuery1.eof) do
    begin
    tempStr := ADOQuery1.fields[0].asstring;    // 读取,方法1
    //tempStr := ADOQuery1.fieldbyname('username').asstring;  //读取,方法2
    ComboBox_name.Items.Add(tempStr);           // 添加到ComboBox_name.Items
    ADOQuery1.next;
    end;
  end;
  ComboBox_name.ItemIndex := 0;
end;

end.
