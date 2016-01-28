#pragma once

#include <Classes.hpp>

#define FORMAT(S, ...) ::Format(S, ##__VA_ARGS__)
#define FMTLOAD(Id, ...) ::FmtLoadStr(Id, ##__VA_ARGS__)

#define FLAGSET(SET, FLAG) (((SET) & (FLAG)) == (FLAG))
#define FLAGCLEAR(SET, FLAG) (((SET) & (FLAG)) == 0)
#define FLAGMASK(ENABLE, FLAG) ((ENABLE) ? (FLAG) : 0)
#define SWAP(TYPE, FIRST, SECOND) \
  { TYPE __Backup = FIRST; FIRST = SECOND; SECOND = __Backup; }

#define PARENTDIRECTORY L".."
#define THISDIRECTORY L"."
#define ROOTDIRECTORY L"/"
#define SLASH L"/"
#define BACKSLASH L"\\"
#define QUOTE L"\'"
#define DOUBLEQUOTE L"\""

enum FileAttributesEnum
{
  faReadOnly = 0x00000001,
  faHidden = 0x00000002,
  faSysFile = 0x00000004,
  faVolumeId = 0x00000008,
  faDirectory = 0x00000010,
  faArchive = 0x00000020,
  faSymLink = 0x00000040,
  faAnyFile = 0x0000003f,
};

intptr_t __cdecl debug_printf(const wchar_t * format, ...);
intptr_t __cdecl debug_printf2(const char * format, ...);

#define NB_TEXT(T) L#T
#ifndef NDEBUG
#if defined(_MSC_VER)
#if (_MSC_VER >= 1900)
#define DEBUG_PRINTF(format, ...) do { ::debug_printf(L"Plugin: [%s:%d] %s: " format L"\n", ::ExtractFilename(__FILEW__, L'\\').c_str(), __LINE__, ::MB2W(__FUNCTION__).c_str(), __VA_ARGS__); } while (0)
#define DEBUG_PRINTF2(format, ...) do { ::debug_printf2("Plugin: [%s:%d] %s: " format "\n", W2MB(::ExtractFilename(__FILEW__, '\\').c_str()).c_str(), __LINE__, __FUNCTION__, __VA_ARGS__); } while (0)
#else
#define DEBUG_PRINTF(format, ...) do { ::debug_printf(L"Plugin: [%s:%d] %s: "NB_TEXT(format) L"\n", ::ExtractFilename(__FILEW__, L'\\').c_str(), __LINE__, ::MB2W(__FUNCTION__).c_str(), __VA_ARGS__); } while (0)
#define DEBUG_PRINTF2(format, ...) do { ::debug_printf2("Plugin: [%s:%d] %s: "format "\n", W2MB(::ExtractFilename(__FILEW__, '\\').c_str()).c_str(), __LINE__, __FUNCTION__, __VA_ARGS__); } while (0)
#endif
#else
#define DEBUG_PRINTF(format, ...) do { ::debug_printf(L"Plugin: [%s:%d] %s: " format L"\n", ::ExtractFilename(MB2W(__FILE__).c_str(), L'\\').c_str(), __LINE__, ::MB2W(__FUNCTION__).c_str(), ##__VA_ARGS__); } while (0)
#define DEBUG_PRINTF2(format, ...) do { ::debug_printf2("Plugin: [%s:%d] %s: " format "\n", W2MB(::ExtractFilename(MB2W(__FILE__).c_str(), '\\').c_str()).c_str(), __LINE__, __FUNCTION__, ##__VA_ARGS__); } while (0)
#endif
#else
#define DEBUG_PRINTF(format, ...)
#define DEBUG_PRINTF2(format, ...)
#endif

UnicodeString MB2W(const char * src, const UINT cp = CP_ACP);
AnsiString W2MB(const wchar_t * src, const UINT cp = CP_ACP);

typedef int TDayTable[12];
extern const TDayTable MonthDays[];

class Exception : public std::runtime_error, public TObject
{
NB_DECLARE_CLASS(Exception)
public:
  explicit Exception(const wchar_t * Msg);
  explicit Exception(const UnicodeString & Msg);
  explicit Exception(Exception * E);
  explicit Exception(std::exception * E);
  explicit Exception(const UnicodeString & Msg, int AHelpContext);
  explicit Exception(Exception * E, int Ident);
  explicit Exception(int Ident);
  ~Exception() throw() {}

public:
  UnicodeString Message;

protected:
  // UnicodeString FHelpKeyword;
};

class EAbort : public Exception
{
NB_DECLARE_CLASS(EAbort)
public:
  explicit EAbort(const UnicodeString & what) : Exception(what)
  {}
};

class EAccessViolation : public Exception
{
NB_DECLARE_CLASS(EAccessViolation)
public:
  explicit EAccessViolation(const UnicodeString & what) : Exception(what)
  {}
};

class EFileNotFoundError : public Exception
{
NB_DECLARE_CLASS(EFileNotFoundError)
public:
  EFileNotFoundError() : Exception(L"")
  {}
};

class EOSError : public Exception
{
NB_DECLARE_CLASS(EOSError)
public:
  explicit EOSError(const UnicodeString & Msg, DWORD code) : Exception(Msg),
    ErrorCode(code)
  {
  }
  DWORD ErrorCode;
};

void RaiseLastOSError(DWORD Result = 0);

struct TFormatSettings : public TObject
{
public:
  explicit TFormatSettings(int /*LCID*/);
  static TFormatSettings Create(int LCID ) { return TFormatSettings(LCID); }
  uint8_t CurrencyFormat;
  uint8_t NegCurrFormat;
  wchar_t ThousandSeparator;
  wchar_t DecimalSeparator;
  uint8_t CurrencyDecimals;
  wchar_t DateSeparator;
  wchar_t TimeSeparator;
  wchar_t ListSeparator;
  UnicodeString CurrencyString;
  UnicodeString ShortDateFormat;
  UnicodeString LongDateFormat;
  UnicodeString TimeAMString;
  UnicodeString TimePMString;
  UnicodeString ShortTimeFormat;
  UnicodeString LongTimeFormat;
  uint16_t TwoDigitYearCenturyWindow;
};

void GetLocaleFormatSettings(int LCID, TFormatSettings & FormatSettings);

UnicodeString ExtractShortPathName(const UnicodeString & APath);
UnicodeString ExtractDirectory(const UnicodeString & APath, wchar_t Delimiter = L'/');
UnicodeString ExtractFilename(const UnicodeString & APath, wchar_t Delimiter = L'/');
UnicodeString ExtractFileExtension(const UnicodeString & APath, wchar_t Delimiter = L'/');
UnicodeString ChangeFileExtension(const UnicodeString & APath, const UnicodeString & Ext, wchar_t Delimiter = L'/');

UnicodeString IncludeTrailingBackslash(const UnicodeString & Str);
UnicodeString ExcludeTrailingBackslash(const UnicodeString & Str);
UnicodeString ExtractFileDir(const UnicodeString & Str);
UnicodeString ExtractFilePath(const UnicodeString & Str);
UnicodeString GetCurrentDir();

UnicodeString IncludeTrailingPathDelimiter(const UnicodeString & Str);

UnicodeString StrToHex(const UnicodeString & Str, bool UpperCase = true, wchar_t Separator = L'\0');
UnicodeString HexToStr(const UnicodeString & Hex);
uintptr_t HexToInt(const UnicodeString & Hex, uintptr_t MinChars = 0);
UnicodeString IntToHex(uintptr_t Int, uintptr_t MinChars = 0);
char HexToChar(const UnicodeString & Hex, uintptr_t MinChars = 0);

UnicodeString ReplaceStrAll(const UnicodeString & Str, const UnicodeString & What, const UnicodeString & ByWhat);
UnicodeString SysErrorMessage(int Code);

bool TryStrToDateTime(const UnicodeString & StrValue, TDateTime & Value, TFormatSettings & FormatSettings);
UnicodeString DateTimeToStr(UnicodeString & Result, const UnicodeString & Format,
  const TDateTime & DateTime);
UnicodeString DateTimeToString(const TDateTime & DateTime);
uint32_t DayOfWeek(const TDateTime & DateTime);

TDateTime Date();
void DecodeDate(const TDateTime & DateTime, uint16_t & Y,
  uint16_t & M, uint16_t & D);
void DecodeTime(const TDateTime & DateTime, uint16_t & H,
  uint16_t & N, uint16_t & S, uint16_t & MS);

UnicodeString FormatDateTime(const UnicodeString & Fmt, const TDateTime & DateTime);
TDateTime SystemTimeToDateTime(const SYSTEMTIME & SystemTime);

TDateTime EncodeDate(int Year, int Month, int Day);
TDateTime EncodeTime(uint32_t Hour, uint32_t Min, uint32_t Sec, uint32_t MSec);

UnicodeString Trim(const UnicodeString & Str);
UnicodeString TrimLeft(const UnicodeString & Str);
UnicodeString TrimRight(const UnicodeString & Str);
UnicodeString UpperCase(const UnicodeString & Str);
UnicodeString LowerCase(const UnicodeString & Str);
wchar_t UpCase(const wchar_t Ch);
wchar_t LowCase(const wchar_t Ch);
UnicodeString AnsiReplaceStr(const UnicodeString & Str, const UnicodeString & From, const UnicodeString & To);
intptr_t AnsiPos(const UnicodeString & Str2, wchar_t Ch);
intptr_t Pos(const UnicodeString & Str2, const UnicodeString & Substr);
UnicodeString StringReplaceAll(const UnicodeString & Str, const UnicodeString & From, const UnicodeString & To);
bool IsDelimiter(const UnicodeString & Delimiters, const UnicodeString & Str, intptr_t AIndex);
intptr_t FirstDelimiter(const UnicodeString & Delimiters, const UnicodeString & Str);
intptr_t LastDelimiter(const UnicodeString & Delimiters, const UnicodeString & Str);

intptr_t CompareText(const UnicodeString & Str1, const UnicodeString & Str2);
intptr_t AnsiCompare(const UnicodeString & Str1, const UnicodeString & Str2);
intptr_t AnsiCompareStr(const UnicodeString & Str1, const UnicodeString & Str2);
bool AnsiSameText(const UnicodeString & Str1, const UnicodeString & Str2);
bool SameText(const UnicodeString & Str1, const UnicodeString & Str2);
intptr_t AnsiCompareText(const UnicodeString & Str1, const UnicodeString & Str2);
intptr_t AnsiCompareIC(const UnicodeString & Str1, const UnicodeString & Str2);
bool AnsiSameStr(const UnicodeString & Str1, const UnicodeString & Str2);
bool AnsiContainsText(const UnicodeString & Str1, const UnicodeString & Str2);
bool ContainsStr(const AnsiString & Str1, const AnsiString & Str2);
bool ContainsText(const UnicodeString & Str1, const UnicodeString & Str2);

UnicodeString UTF8ToString(const RawByteString & Str);
UnicodeString UTF8ToString(const char * Str, intptr_t Len);

int StringCmp(const wchar_t * S1, const wchar_t * S2);
int StringCmpI(const wchar_t * S1, const wchar_t * S2);

UnicodeString IntToStr(intptr_t Value);
UnicodeString Int64ToStr(int64_t Value);
intptr_t StrToInt(const UnicodeString & Value);
int64_t ToInt(const UnicodeString & Value);
intptr_t StrToIntDef(const UnicodeString & Value, intptr_t DefVal);
int64_t StrToInt64(const UnicodeString & Value);
int64_t StrToInt64Def(const UnicodeString & Value, int64_t DefVal);
bool TryStrToInt(const UnicodeString & StrValue, int64_t & Value);

double StrToFloat(const UnicodeString & Value);
double StrToFloatDef(const UnicodeString & Value, double DefVal);
UnicodeString FormatFloat(const UnicodeString & Format, double Value);
bool IsZero(double Value);

TTimeStamp DateTimeToTimeStamp(const TDateTime & DateTime);

int64_t FileRead(HANDLE Handle, void * Buffer, int64_t Count);
int64_t FileWrite(HANDLE Handle, const void * Buffer, int64_t Count);
int64_t FileSeek(HANDLE Handle, int64_t Offset, DWORD Origin);

bool FileExists(const UnicodeString & AFileName);
bool RenameFile(const UnicodeString & From, const UnicodeString & To);
bool DirectoryExists(const UnicodeString & ADir);
UnicodeString FileSearch(const UnicodeString & AFileName, const UnicodeString & DirectoryList);
void FileAge(const UnicodeString & AFileName, TDateTime & ATimestamp);

DWORD FileGetAttr(const UnicodeString & AFileName);
DWORD FileSetAttr(const UnicodeString & AFileName, DWORD LocalFileAttrs);

bool ForceDirectories(const UnicodeString & ADir);
bool RemoveFile(const UnicodeString & AFileName);
bool CreateDir(const UnicodeString & ADir);
bool RemoveDir(const UnicodeString & ADir);

UnicodeString Format(const wchar_t * Format, ...);
UnicodeString FormatV(const wchar_t * Format, va_list Args);
AnsiString FormatA(const char * Format, ...);
AnsiString FormatA(const char * Format, va_list Args);
UnicodeString FmtLoadStr(intptr_t Id, ...);

UnicodeString WrapText(const UnicodeString & Line, intptr_t MaxWidth = 40);

UnicodeString TranslateExceptionMessage(Exception * E);

void AppendWChar(UnicodeString & Str2, const wchar_t Ch);
void AppendChar(std::string & Str2, const char Ch);

void AppendPathDelimiterW(UnicodeString & Str);

UnicodeString ExpandEnvVars(const UnicodeString & Str);

UnicodeString StringOfChar(const wchar_t Ch, intptr_t Len);

UnicodeString ChangeFileExt(const UnicodeString & AFileName, const UnicodeString & AExt);
UnicodeString ExtractFileExt(const UnicodeString & AFileName);
UnicodeString ExpandUNCFileName(const UnicodeString & AFileName);

typedef WIN32_FIND_DATA TWin32FindData;
typedef UnicodeString TFileName;

struct TSystemTime
{
  Word wYear;
  Word wMonth;
  Word wDayOfWeek;
  Word wDay;
  Word wHour;
  Word wMinute;
  Word wSecond;
  Word wMilliseconds;
};

struct TFileTime
{
  Integer LowTime;
  Integer HighTime;
};

struct TSearchRec : public TObject
{
NB_DISABLE_COPY(TSearchRec)
public:
  TSearchRec() :
    Time(0),
    Size(0),
    Attr(0),
    ExcludeAttr(0),
    FindHandle(INVALID_HANDLE_VALUE)
  {
    ClearStruct(FindData);
  }
  Integer Time;
  Int64 Size;
  Integer Attr;
  TFileName Name;
  Integer ExcludeAttr;
  THandle FindHandle;
  TWin32FindData FindData;
};

DWORD FindFirst(const UnicodeString & AFileName, DWORD LocalFileAttrs, TSearchRec & Rec);
DWORD FindNext(TSearchRec & Rec);
DWORD FindClose(TSearchRec & Rec);

void InitPlatformId();
bool Win32Check(bool RetVal);

class EConvertError : public Exception
{
public:
  explicit EConvertError(const UnicodeString & Msg) :
    Exception(Msg)
  {}
};

UnicodeString UnixExcludeLeadingBackslash(const UnicodeString & APath);

extern int RandSeed;
extern void Randomize();

TDateTime IncYear(const TDateTime & AValue, const Int64 ANumberOfYears = 1);
TDateTime IncMonth(const TDateTime & AValue, const Int64 NumberOfMonths = 1);
TDateTime IncWeek(const TDateTime & AValue, const Int64 ANumberOfWeeks = 1);
TDateTime IncDay(const TDateTime & AValue, const Int64 ANumberOfDays = 1);
TDateTime IncHour(const TDateTime & AValue, const Int64 ANumberOfHours = 1);
TDateTime IncMinute(const TDateTime & AValue, const Int64 ANumberOfMinutes = 1);
TDateTime IncSecond(const TDateTime & AValue, const Int64 ANumberOfSeconds = 1);
TDateTime IncMilliSecond(const TDateTime & AValue, const Int64 ANumberOfMilliSeconds = 1);

Boolean IsLeapYear(Word Year);

class TCriticalSection : public TObject
{
public:
  TCriticalSection();
  ~TCriticalSection();

  void Enter() const;
  void Leave() const;

  int GetAcquired() const { return FAcquired; }

private:
  mutable CRITICAL_SECTION FSection;
  mutable int FAcquired;
};

UnicodeString StripHotkey(const UnicodeString & AText);
bool StartsText(const UnicodeString & ASubText, const UnicodeString & AText);

struct TVersionInfo
{
  DWORD Major;
  DWORD Minor;
  DWORD Revision;
  DWORD Build;
};

#define MAKEVERSIONNUMBER(major, minor, revision) ( ((major)<<16) | ((minor)<<8) | (revision))
uintptr_t StrToVersionNumber(const UnicodeString & VersionMumberStr);
UnicodeString VersionNumberToStr(uintptr_t VersionNumber);
uintptr_t inline GetVersionNumber219() { return MAKEVERSIONNUMBER(2,1,9); }
uintptr_t inline GetVersionNumber2110() { return MAKEVERSIONNUMBER(2,1,10); }
uintptr_t inline GetVersionNumber2121() { return MAKEVERSIONNUMBER(2,1,21); }
uintptr_t inline GetCurrentVersionNumber() { return StrToVersionNumber(GetGlobalFunctions()->GetStrVersionNumber()); }

class ScopeExit
{
public:
  ScopeExit(const std::function<void()> & f) : m_f(f) {}
  ~ScopeExit() { m_f(); }

private:
  std::function<void()> m_f;
};

#define _SCOPE_EXIT_NAME(name, suffix) name ## suffix
#define SCOPE_EXIT_NAME(name, suffix) _SCOPE_EXIT_NAME(name, suffix)
#define SCOPE_EXIT \
  std::function<void()> SCOPE_EXIT_NAME(scope_exit_func_, __LINE__); \
  ScopeExit SCOPE_EXIT_NAME(scope_exit_, __LINE__) = SCOPE_EXIT_NAME(scope_exit_func_, __LINE__) = [&]() /* lambda body here */

class NullFunc
{
public:
  NullFunc(const std::function<void()> & f) { (void)(f); }
  ~NullFunc() { }
};

#define try__catch (void)0;
#define try__finally (void)0;

#define __finally \
  std::function<void()> SCOPE_EXIT_NAME(null_func_, __LINE__); \
  NullFunc SCOPE_EXIT_NAME(null_, __LINE__) = SCOPE_EXIT_NAME(null_func_, __LINE__) = [&]() /* lambda body here */

