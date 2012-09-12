//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <Common.h>
#include <RemoteFiles.h>
#include <Terminal.h>
#include <DiscMon.hpp>
#include <Exceptions.h>
#include "GUIConfiguration.h"
#include "CoreMain.h"
#include "TextsCore.h"
#include "SynchronizeController.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
/* __fastcall */ TSynchronizeController::TSynchronizeController(
  TSynchronizeEvent AOnSynchronize, TSynchronizeInvalidEvent AOnSynchronizeInvalid,
  TSynchronizeTooManyDirectoriesEvent AOnTooManyDirectories)
{
  FOnSynchronize = AOnSynchronize;
  FOnSynchronizeInvalid = AOnSynchronizeInvalid;
  FOnTooManyDirectories = AOnTooManyDirectories;
  FSynchronizeMonitor = NULL;
  FSynchronizeAbort = NULL;
  FSynchronizeLog = NULL;
  FOptions = NULL;
  FOnSynchronizeThreads = NULL;
}
//---------------------------------------------------------------------------
/* __fastcall */ TSynchronizeController::~TSynchronizeController()
{
  assert(FSynchronizeMonitor == NULL);
}
//---------------------------------------------------------------------------
void /* __fastcall */ TSynchronizeController::StartStop(TObject * Sender,
  bool Start, const TSynchronizeParamType & Params, const TCopyParamType & CopyParam,
  TSynchronizeOptions * Options,
  TSynchronizeAbortEvent OnAbort, TSynchronizeThreadsEvent OnSynchronizeThreads,
  TSynchronizeLogEvent OnSynchronizeLog)
{
  if (Start)
  {
    // Configuration->Usage->Inc(L"KeepUpToDates");

    try
    {
      assert(OnSynchronizeLog != NULL);
      FSynchronizeLog = OnSynchronizeLog;

      FOptions = Options;
      if (FLAGSET(Params.Options, soSynchronize) &&
          (FOnSynchronize != NULL))
      {
        FOnSynchronize(this, Params.LocalDirectory,
          Params.RemoteDirectory, CopyParam,
          Params, NULL, FOptions, true);
      }

      FCopyParam = CopyParam;
      FSynchronizeParams = Params;

      assert(OnAbort);
      FSynchronizeAbort = OnAbort;

      if (FLAGSET(FSynchronizeParams.Options, soRecurse))
      {
        SynchronizeLog(slScan,
          FMTLOAD(SYNCHRONIZE_SCAN, FSynchronizeParams.LocalDirectory.c_str()));
      }
      Error(SNotImplemented, 256);
      /*
      // FIXME
      FSynchronizeMonitor = new TDiscMonitor(dynamic_cast<TComponent*>(Sender));
      FSynchronizeMonitor->SubTree = false;
      TMonitorFilters Filters;
      Filters << moFilename << moLastWrite;
      if (FLAGSET(FSynchronizeParams.Options, soRecurse))
      {
        Filters << moDirName;
      }
      FSynchronizeMonitor->Filters = Filters;
      FSynchronizeMonitor->MaxDirectories = 0;
      FSynchronizeMonitor->ChangeDelay = GUIConfiguration->GetKeepUpToDateChangeDelay();
      FSynchronizeMonitor->OnTooManyDirectories = SynchronizeTooManyDirectories;
      FSynchronizeMonitor->OnDirectoriesChange = SynchronizeDirectoriesChange;
      FSynchronizeMonitor->OnFilter = SynchronizeFilter;
      FSynchronizeMonitor->AddDirectory(FSynchronizeParams.LocalDirectory,
        FLAGSET(FSynchronizeParams.Options, soRecurse));
      FSynchronizeMonitor->OnChange = SynchronizeChange;
      FSynchronizeMonitor->OnInvalid = SynchronizeInvalid;
      FSynchronizeMonitor->OnSynchronize = OnSynchronizeThreads;
      // get count before open to avoid thread issues
      int Directories = FSynchronizeMonitor->Directories->GetCount();
      FSynchronizeMonitor->Open();
      SynchronizeLog(slStart, FMTLOAD(SYNCHRONIZE_START, GetDirectories()));
      */
    }
    catch(...)
    {
      // FIXME SAFE_DESTROY((TObject *)FSynchronizeMonitor);
      Error(SNotImplemented, 257);
      throw;
    }
  }
  else
  {
    FOptions = NULL;
    // SAFE_DESTROY((TObject *)FSynchronizeMonitor);
  }
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeController::SynchronizeChange(
  TObject * /*Sender*/, const UnicodeString Directory, bool & SubdirsChanged)
{
  try
  {
    UnicodeString RemoteDirectory;
    UnicodeString RootLocalDirectory;
    RootLocalDirectory = IncludeTrailingBackslash(FSynchronizeParams.LocalDirectory);
    RemoteDirectory = UnixIncludeTrailingBackslash(FSynchronizeParams.RemoteDirectory);

    UnicodeString LocalDirectory = IncludeTrailingBackslash(Directory);

    assert(LocalDirectory.SubString(1, RootLocalDirectory.Length()) ==
      RootLocalDirectory);
    RemoteDirectory = RemoteDirectory +
      ToUnixPath(LocalDirectory.SubString(RootLocalDirectory.Length() + 1,
        LocalDirectory.Length() - RootLocalDirectory.Length()));

    SynchronizeLog(slChange, FMTLOAD(SYNCHRONIZE_CHANGE,
      ExcludeTrailingBackslash(LocalDirectory).c_str()));

    if (FOnSynchronize != NULL)
    {
      // this is completelly wrong as the options structure
      // can contain non-root specific options in future
      TSynchronizeOptions * Options =
        ((LocalDirectory == RootLocalDirectory) ? FOptions : NULL);
      TSynchronizeChecklist * Checklist = NULL;
      FOnSynchronize(this, LocalDirectory, RemoteDirectory, FCopyParam,
        FSynchronizeParams, &Checklist, Options, false);
      if (Checklist != NULL)
      {
        std::auto_ptr<TSynchronizeChecklist> ChecklistPtr(Checklist);
        {
          if (FLAGSET(FSynchronizeParams.Options, soRecurse))
          {
            SubdirsChanged = false;
            assert(Checklist != NULL);
            for (int Index = 0; Index < Checklist->GetCount(); Index++)
            {
              const TSynchronizeChecklist::TItem * Item = Checklist->GetItem(Index);
              // note that there may be action saDeleteRemote even if nothing has changed
              // so this is sub-optimal
              if (Item->IsDirectory)
              {
                if ((Item->Action == TSynchronizeChecklist::saUploadNew) ||
                    (Item->Action == TSynchronizeChecklist::saDeleteRemote))
                {
                  SubdirsChanged = true;
                  break;
                }
                else
                {
                  assert(false);
                }
              }
            }
          }
          else
          {
            SubdirsChanged = false;
          }
        }
      }
    }
  }
  catch(Exception & E)
  {
    SynchronizeAbort(dynamic_cast<EFatal*>(&E) != NULL);
  }
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeController::SynchronizeAbort(bool Close)
{
  if (FSynchronizeMonitor != NULL)
  {
    // FIXME FSynchronizeMonitor->Close();
    Error(SNotImplemented, 258);
  }
  assert(FSynchronizeAbort);
  FSynchronizeAbort(NULL, Close);
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeController::LogOperation(TSynchronizeOperation Operation,
  const UnicodeString FileName)
{
  TSynchronizeLogEntry Entry;
  UnicodeString Message;
  switch (Operation)
  {
    case soDelete:
      Entry = slDelete;
      Message = FMTLOAD(SYNCHRONIZE_DELETED, FileName.c_str());
      break;

    default:
      assert(false);
      // fallthru

    case soUpload:
      Entry = slUpload;
      Message = FMTLOAD(SYNCHRONIZE_UPLOADED, FileName.c_str());
      break;
  }
  SynchronizeLog(Entry, Message);
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeController::SynchronizeLog(TSynchronizeLogEntry Entry,
  const UnicodeString Message)
{
  if (FSynchronizeLog != NULL)
  {
    FSynchronizeLog(this, Entry, Message);
  }
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeController::SynchronizeFilter(TObject * /*Sender*/,
  const UnicodeString DirectoryName, bool & Add)
{
  if ((FOptions != NULL) && (FOptions->Filter != NULL))
  {
    if (IncludeTrailingBackslash(ExtractFilePath(DirectoryName)) ==
          IncludeTrailingBackslash(FSynchronizeParams.LocalDirectory))
    {
      int FoundIndex;
      Add = FOptions->Filter->Find(ExtractFileName(DirectoryName, true), FoundIndex);
    }
  }
  TFileMasks::TParams MaskParams; // size/time does not matter for directories
  Add = Add && FCopyParam.AllowTransfer(DirectoryName, osLocal, true, MaskParams);
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeController::SynchronizeInvalid(
  TObject * /*Sender*/, const UnicodeString Directory, const UnicodeString ErrorStr)
{
  if (FOnSynchronizeInvalid != NULL)
  {
    FOnSynchronizeInvalid(this, Directory, ErrorStr);
  }

  SynchronizeAbort(false);
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeController::SynchronizeTooManyDirectories(
  TObject * /*Sender*/, int & MaxDirectories)
{
  if (FOnTooManyDirectories != NULL)
  {
    FOnTooManyDirectories(this, MaxDirectories);
  }
}
//---------------------------------------------------------------------------
void __fastcall TSynchronizeController::SynchronizeDirectoriesChange(
  TObject * /*Sender*/, int Directories)
{
  SynchronizeLog(slDirChange, FMTLOAD(SYNCHRONIZE_START, Directories));
}
