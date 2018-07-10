object Form_SerialSet: TForm_SerialSet
  Left = 299
  Top = 302
  Width = 244
  Height = 210
  Caption = #20018#21475#35774#32622
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 209
    Height = 153
    TabOrder = 0
    object Label_Num: TLabel
      Left = 40
      Top = 24
      Width = 40
      Height = 16
      Caption = #20018#21475':'
      Font.Charset = GB2312_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = #23435#20307
      Font.Style = []
      ParentFont = False
      Transparent = True
    end
    object Label_Bps: TLabel
      Left = 24
      Top = 66
      Width = 56
      Height = 16
      Caption = #27874#29305#29575':'
      Font.Charset = GB2312_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = #23435#20307
      Font.Style = []
      ParentFont = False
      Transparent = True
    end
    object Button_Ok: TButton
      Left = 32
      Top = 112
      Width = 67
      Height = 25
      Caption = #30830#23450
      TabOrder = 0
      OnClick = Button_OkClick
    end
    object Button_Quit: TButton
      Left = 120
      Top = 112
      Width = 65
      Height = 25
      Caption = #21462#28040
      TabOrder = 1
      OnClick = Button_QuitClick
    end
    object ComboBox_Num: TComboBox
      Left = 96
      Top = 24
      Width = 89
      Height = 21
      ItemHeight = 13
      TabOrder = 2
      Text = 'COM1'
      Items.Strings = (
        'COM1'
        'COM2'
        'COM3'
        'COM4'
        'COM5'
        'COM6'
        'COM7'
        'COM8'
        'COM9'
        'COM10'
        'COM11'
        'COM12'
        'COM13'
        'COM14'
        'COM15'
        'COM16'
        'COM17'
        'COM18'
        'COM19'
        'COM20'
        'COM21'
        'COM22'
        'COM23'
        'COM24'
        'COM25'
        'COM26'
        'COM27'
        'COM28'
        'COM29'
        'COM30'
        'COM31'
        'COM32'
        'COM33'
        'COM34'
        'COM35'
        'COM36'
        'COM37'
        'COM38'
        'COM39'
        'COM40')
    end
    object ComboBox_Bps: TComboBox
      Left = 96
      Top = 64
      Width = 89
      Height = 21
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 3
      Text = '1200'
      Items.Strings = (
        '1200'
        '2400'
        '4800'
        '9600'
        '14400'
        '19200'
        '28800'
        '38400'
        '57600'
        '115200')
    end
  end
  object Comm1: TComm
    CommName = 'COM2'
    BaudRate = 9600
    ParityCheck = False
    Outx_CtsFlow = False
    Outx_DsrFlow = False
    DtrControl = DtrEnable
    DsrSensitivity = False
    TxContinueOnXoff = True
    Outx_XonXoffFlow = True
    Inx_XonXoffFlow = True
    ReplaceWhenParityError = False
    IgnoreNullChar = False
    RtsControl = RtsEnable
    XonLimit = 500
    XoffLimit = 500
    ByteSize = _8
    Parity = None
    StopBits = _1
    XonChar = #17
    XoffChar = #19
    ReplacedChar = #0
    ReadIntervalTimeout = 100
    ReadTotalTimeoutMultiplier = 0
    ReadTotalTimeoutConstant = 0
    WriteTotalTimeoutMultiplier = 0
    WriteTotalTimeoutConstant = 0
    OnReceiveData = Comm1ReceiveData
    Left = 16
    Top = 40
  end
  object ADOQuery1: TADOQuery
    Connection = Form_main.ADOConnection1
    CursorType = ctStatic
    Parameters = <>
    SQL.Strings = (
      'select * from  login_info')
    Left = 16
    Top = 96
  end
end
