object Form_main: TForm_main
  Left = 582
  Top = 244
  Width = 907
  Height = 414
  Caption = #39068#33394#39057#29575#21015#34920
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010001002020040000000000E80200001600000028000000200000004000
    0000010004000000000000020000000000000000000000000000000000000000
    0000000080000080000000808000800000008000800080800000C0C0C0008080
    80000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000000
    00000000000000000000000000000000000000000000000000000008F7000000
    000000000000000000000008F7000000000000000000000000000008F7700000
    0000000000000000000000008F7000000000000000000000000000008F700000
    00000007FFF70000000000008F700000000007FF777FF700000000008F700000
    00007F7778877F7000000008F77000000087F778800877F700000008F7700000
    008F77800000877F70000887F700000008F7700000000877F70007FF77000000
    08F77000000000877FFFFF777000000008F77000000000087777777880000000
    008F7700000000008888888000000000008F7700000000000000000000000000
    088F778000000000000000000000000087F77778000000000000000000000008
    7F777777800000000000000000000087F777777778000000000000000000087F
    7777777777800000000000000000087F7888888887800000000000000000087F
    7FFFFFFFF7800000000000000000087F7888888887800000000000000000087F
    7FFFFFFFF7800000000000000000087F77777777778000000000000000000888
    8888888888800000000000000000008F3333333333000000000000000000008F
    3333333333000000000000000000008FBFBFBFBFBF0000000000000000000088
    888888888800000000000000000000000000000000000000000000000000FFFF
    FFE1FFFFFFE1FFFFFFE1FFFFFFE0FFFFFFF0FFE0FFF0FF803FF0FF001FF0FE00
    0FE0FC0607E0FC1F0381F83F8001F83FC003F83FE007FC1FF01FFC1FFFFFF80F
    FFFFF007FFFFE003FFFFC001FFFF8000FFFF8000FFFF8000FFFF8000FFFF8000
    FFFF8000FFFF8000FFFFC001FFFFC001FFFFC001FFFFC001FFFFFFFFFFFF}
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 16
  object DBGrid1: TDBGrid
    Left = 0
    Top = 0
    Width = 891
    Height = 376
    Align = alClient
    DataSource = DataSource1
    Options = [dgTitles, dgIndicator, dgColumnResize, dgColLines, dgRowLines, dgTabs, dgRowSelect, dgConfirmDelete, dgCancelOnExit]
    PopupMenu = PopupMenu1
    TabOrder = 0
    TitleFont.Charset = DEFAULT_CHARSET
    TitleFont.Color = clWindowText
    TitleFont.Height = -13
    TitleFont.Name = 'MS Sans Serif'
    TitleFont.Style = []
    Columns = <
      item
        Expanded = False
        FieldName = 'id'
        Visible = False
      end
      item
        Expanded = False
        FieldName = 'dot'
        Width = 188
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'freq1'
        Width = 171
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'freq2'
        Width = 171
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'freq3'
        Width = 137
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'freq4'
        Width = 116
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'dates'
        Visible = True
      end
      item
        Expanded = False
        FieldName = 'times'
        Visible = True
      end>
  end
  object PopupMenu1: TPopupMenu
    Left = 192
    Top = 216
    object N1: TMenuItem
      Caption = #21478#23384#20026
      OnClick = N1Click
    end
    object N2: TMenuItem
      Caption = #20840#37096#28165#38500
      OnClick = N2Click
    end
  end
  object ADOConnection1: TADOConnection
    ConnectionString = 
      'Provider=Microsoft.Jet.OLEDB.4.0;User ID=Admin;Data Source=E:\ZY' +
      'DTechDirectory\ZYDDevelop\ZYD879PulseCheck\Soft1701181258\PC1701' +
      '182216\Data.mdb;Mode=Share Deny None;Persist Security Info=False' +
      ';Jet OLEDB:System database="";Jet OLEDB:Registry Path="";Jet OLE' +
      'DB:Database Password="";Jet OLEDB:Engine Type=5;Jet OLEDB:Databa' +
      'se Locking Mode=1;Jet OLEDB:Global Partial Bulk Ops=2;Jet OLEDB:' +
      'Global Bulk Transactions=1;Jet OLEDB:New Database Password="";Je' +
      't OLEDB:Create System Database=False;Jet OLEDB:Encrypt Database=' +
      'False;Jet OLEDB:Don'#39't Copy Locale on Compact=False;Jet OLEDB:Com' +
      'pact Without Replica Repair=False;Jet OLEDB:SFP=False;'
    LoginPrompt = False
    Provider = 'Microsoft.Jet.OLEDB.4.0'
    Left = 192
    Top = 160
  end
  object ADOQuery1: TADOQuery
    Connection = ADOConnection1
    Parameters = <>
    Left = 248
    Top = 160
  end
  object DataSource1: TDataSource
    DataSet = ADOQuery1
    Left = 360
    Top = 160
  end
  object Timer2: TTimer
    Enabled = False
    Interval = 50
    OnTimer = Timer2Timer
    Left = 304
    Top = 216
  end
  object ADOQuery2: TADOQuery
    Connection = ADOConnection1
    Parameters = <>
    Left = 304
    Top = 160
  end
  object SaveDialog1: TSaveDialog
    FileName = 'fault'
    Filter = 'xlsx'
    Left = 408
    Top = 160
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 300
    OnTimer = Timer1Timer
    Left = 248
    Top = 216
  end
end
