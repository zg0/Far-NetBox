//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "CoreMain.h"

#include "Common.h"
#include "Interface.h"
#include "Configuration.h"
#include "PuttyIntf.h"
#include "Cryptography.h"
#ifndef NO_FILEZILLA
#include "FileZillaIntf.h"
#endif
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
TStoredSessionList * StoredSessions = NULL;
//---------------------------------------------------------------------------
TConfiguration * GetConfiguration()
{
  static TConfiguration * Configuration = NULL;
  if (Configuration == NULL)
  {
    // configuration needs to be created and loaded before putty is initialized,
    // so that random seed path is known
    Configuration = CreateConfiguration();
    try
    {
      Configuration->Load();
    }
    catch (Exception & E)
    {
      ShowExtendedException(&E);
    }

    PuttyInitialize();
  }
  return Configuration;
}
//---------------------------------------------------------------------------
void DeleteConfiguration()
{
  static bool ConfigurationDeleted = false;
  if (!ConfigurationDeleted)
  {
    delete GetConfiguration();
    ConfigurationDeleted = true;
  }
}

//---------------------------------------------------------------------------
TQueryButtonAlias::TQueryButtonAlias() :
  Button(0)
{
  OnClick = NULL;
}
//---------------------------------------------------------------------------
TQueryParams::TQueryParams(uintptr_t AParams, const UnicodeString & AHelpKeyword)
{
  Params = AParams;
  Aliases = NULL;
  AliasesCount = 0;
  Timer = 0;
  TimerEvent = NULL;
  TimerMessage = L"";
  TimerAnswers = 0;
  Timeout = 0;
  TimeoutAnswer = 0;
  NoBatchAnswers = 0;
  HelpKeyword = AHelpKeyword;
}
//---------------------------------------------------------------------------
bool IsAuthenticationPrompt(TPromptKind Kind)
{
  return
    (Kind == pkUserName) || (Kind == pkPassphrase) || (Kind == pkTIS) ||
    (Kind == pkCryptoCard) || (Kind == pkKeybInteractive) ||
    (Kind == pkPassword) || (Kind == pkNewPassword);
}
//---------------------------------------------------------------------------
void CoreInitialize()
{
  Randomize();
  CryptographyInitialize();

  #ifndef NO_FILEZILLA
  TFileZillaIntf::Initialize();
  #endif

  StoredSessions = new TStoredSessionList();

  try
  {
    StoredSessions->Load();
  }
  catch (Exception & E)
  {
    ShowExtendedException(&E);
  }
}
//---------------------------------------------------------------------------
void CoreFinalize()
{
  try
  {
    // only modified, implicit
    GetConfiguration()->Save(false, false);
  }
  catch(Exception & E)
  {
    ShowExtendedException(&E);
  }

  #ifndef NO_FILEZILLA
  TFileZillaIntf::Finalize();
  #endif
  PuttyFinalize();

  delete StoredSessions;
  StoredSessions = NULL;
  DeleteConfiguration();

  CryptographyFinalize();
}
//---------------------------------------------------------------------------
void CoreSetResourceModule(void * ResourceHandle)
{
  #ifndef NO_FILEZILLA
  TFileZillaIntf::SetResourceModule(ResourceHandle);
  #else
  USEDPARAM(ResourceHandle);
  #endif
}
//---------------------------------------------------------------------------
void CoreMaintenanceTask()
{
  DontSaveRandomSeed();
}
//---------------------------------------------------------------------------
