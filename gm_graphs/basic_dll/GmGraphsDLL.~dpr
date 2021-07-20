library GmGraphsDLL;

{ Important note about DLL memory management: ShareMem must be the
  first unit in your library's USES clause AND your project's (select
  Project-View Source) USES clause if your DLL exports any procedures or
  functions that pass strings as parameters or function results. This
  applies to all strings passed to and from your DLL--even those that
  are nested in records and classes. ShareMem is the interface unit to
  the BORLNDMM.DLL shared memory manager, which must be deployed along
  with your DLL. To avoid using BORLNDMM.DLL, pass string information
  using PChar or ShortString parameters. }

uses
  SysUtils,
  Classes,
  Dialogs;

{$R *.res}


var stored_str : string;

function dFileExists(code1, code2 : double) : double; cdecl;
var fe : boolean;
begin      
  Result := 0;
  
  if (code1 = 0) then begin
    stored_str := '';
    Result := 0;
    exit;
  end;

  if (code1 = 1) then begin
    stored_str := stored_str + chr(trunc(code2));
  end;

  if (code1 = 2) then begin
    fe := FileExists(stored_str);
    if (fe) then Result := 1 else Result := 0;
    //ShowMessage('2 - saved str = ' + chr(10) + stored_str + chr(10) + ' exists = ' + booltostr(fe, true)
    //+ chr(10) + 'res = ' + floattostr(Result));
  end;

  //FileExists
end;

exports dFileExists;

begin
end.
 