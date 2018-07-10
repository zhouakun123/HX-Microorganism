program colorFreq;

uses
  Forms,
  common in 'common.pas',
  Login in 'Login.pas' {Form_Login},
  main in 'main.pas' {Form_main},
  SerialSet in 'SerialSet.pas' {Form_SerialSet};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TForm_Login, Form_Login);
  Application.CreateForm(TForm_main, Form_main);
  Application.CreateForm(TForm_SerialSet, Form_SerialSet);
  Application.Run;
end.
