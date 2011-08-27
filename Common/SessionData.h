//---------------------------------------------------------------------------
#ifndef SessionDataH
#define SessionDataH

#include "Common.h"
#include "Option.h"
#include "FileBuffer.h"
#include "NamedObjs.h"
#include "HierarchicalStorage.h"
#include "Configuration.h"
//---------------------------------------------------------------------------
#define SET_SESSION_PROPERTY(Property) \
  if (F##Property != value) { F##Property = value; Modify(); }
//---------------------------------------------------------------------------
enum TCipher { cipWarn, cip3DES, cipBlowfish, cipAES, cipDES, cipArcfour };
#define CIPHER_COUNT (cipArcfour+1)
enum TProtocol { ptRaw, ptTelnet, ptRLogin, ptSSH };
#define PROTOCOL_COUNT (ptSSH+1)
// explicit values to skip obsoleted fsExternalSSH, fsExternalSFTP
enum TFSProtocol { fsSCPonly = 0, fsSFTP = 1, fsSFTPonly = 2, fsFTP = 5 };
#define FSPROTOCOL_COUNT (fsFTP+1)
enum TProxyMethod { pmNone, pmSocks4, pmSocks5, pmHTTP, pmTelnet, pmCmd };
enum TSshProt { ssh1only, ssh1, ssh2, ssh2only };
enum TKex { kexWarn, kexDHGroup1, kexDHGroup14, kexDHGEx, kexRSA };
#define KEX_COUNT (kexRSA+1)
enum TSshBug { sbIgnore1, sbPlainPW1, sbRSA1, sbHMAC2, sbDeriveKey2, sbRSAPad2,
  sbRekey2, sbPKSessID2, sbMaxPkt2 };
#define BUG_COUNT (sbMaxPkt2+1)
enum TSftpBug { sbSymlink, sbSignedTS };
#define SFTP_BUG_COUNT (sbSignedTS+1)
enum TPingType { ptOff, ptNullPacket, ptDummyCommand };
enum TAddressFamily { afAuto, afIPv4, afIPv6 };
enum TFtps { ftpsNone, ftpsImplicit, ftpsExplicitSsl, ftpsExplicitTls };
enum TSessionSource { ssNone, ssStored, ssStoredModified };
//---------------------------------------------------------------------------
extern const wchar_t CipherNames[CIPHER_COUNT][10];
extern const wchar_t KexNames[KEX_COUNT][20];
extern const wchar_t ProtocolNames[PROTOCOL_COUNT][10];
extern const wchar_t SshProtList[][10];
extern const wchar_t ProxyMethodList[][10];
extern const TCipher DefaultCipherList[CIPHER_COUNT];
extern const TKex DefaultKexList[KEX_COUNT];
extern const wchar_t FSProtocolNames[FSPROTOCOL_COUNT][11];
//---------------------------------------------------------------------------
class TStoredSessionList;
//---------------------------------------------------------------------------
class TSessionData : public TNamedObject
{
friend class TStoredSessionList;

private:
  std::wstring FHostName;
  int FPortNumber;
  std::wstring FUserName;
  std::wstring FPassword;
  bool FPasswordless;
  int FPingInterval;
  TPingType FPingType;
  bool FTryAgent;
  bool FAgentFwd;
  std::wstring FListingCommand;
  bool FAuthTIS;
  bool FAuthKI;
  bool FAuthKIPassword;
  bool FAuthGSSAPI;
  bool FGSSAPIFwdTGT; // not supported anymore
  std::wstring FGSSAPIServerRealm; // not supported anymore
  bool FChangeUsername;
  bool FCompression;
  TSshProt FSshProt;
  bool FSsh2DES;
  bool FSshNoUserAuth;
  TCipher FCiphers[CIPHER_COUNT];
  TKex FKex[KEX_COUNT];
  bool FClearAliases;
  TEOLType FEOLType;
  std::wstring FPublicKeyFile;
  TProtocol FProtocol;
  TFSProtocol FFSProtocol;
  bool FModified;
  std::wstring FLocalDirectory;
  std::wstring FRemoteDirectory;
  bool FLockInHome;
  bool FSpecial;
  bool FUpdateDirectories;
  bool FCacheDirectories;
  bool FCacheDirectoryChanges;
  bool FPreserveDirectoryChanges;
  bool FSelected;
  bool FLookupUserGroups;
  std::wstring FReturnVar;
  bool FScp1Compatibility;
  std::wstring FShell;
  std::wstring FSftpServer;
  int FTimeout;
  bool FUnsetNationalVars;
  bool FIgnoreLsWarnings;
  bool FTcpNoDelay;
  TProxyMethod FProxyMethod;
  std::wstring FProxyHost;
  int FProxyPort;
  std::wstring FProxyUsername;
  std::wstring FProxyPassword;
  std::wstring FProxyTelnetCommand;
  std::wstring FProxyLocalCommand;
  TAutoSwitch FProxyDNS;
  bool FProxyLocalhost;
  int FFtpProxyLogonType;
  TAutoSwitch FBugs[BUG_COUNT];
  std::wstring FCustomParam1;
  std::wstring FCustomParam2;
  bool FResolveSymlinks;
  TDateTime FTimeDifference;
  int FSFTPDownloadQueue;
  int FSFTPUploadQueue;
  int FSFTPListingQueue;
  int FSFTPMaxVersion;
  unsigned long FSFTPMaxPacketSize;
  TDSTMode FDSTMode;
  TAutoSwitch FSFTPBugs[SFTP_BUG_COUNT];
  bool FDeleteToRecycleBin;
  bool FOverwrittenToRecycleBin;
  std::wstring FRecycleBinPath;
  std::wstring FPostLoginCommands;
  TAutoSwitch FSCPLsFullTime;
  TAutoSwitch FFtpListAll;
  TAddressFamily FAddressFamily;
  std::wstring FRekeyData;
  unsigned int FRekeyTime;
  int FColor;
  bool FTunnel;
  std::wstring FTunnelHostName;
  int FTunnelPortNumber;
  std::wstring FTunnelUserName;
  std::wstring FTunnelPassword;
  std::wstring FTunnelPublicKeyFile;
  int FTunnelLocalPortNumber;
  std::wstring FTunnelPortFwd;
  bool FFtpPasvMode;
  bool FFtpForcePasvIp;
  std::wstring FFtpAccount;
  int FFtpPingInterval;
  TPingType FFtpPingType;
  TFtps FFtps;
  TAutoSwitch FNotUtf;
  std::wstring FHostKey;

  std::wstring FOrigHostName;
  int FOrigPortNumber;
  TProxyMethod FOrigProxyMethod;
  TSessionSource FSource;

  void SavePasswords(THierarchicalStorage * Storage, bool PuttyExport);
  void Modify();
  static std::wstring EncryptPassword(const std::wstring & Password, std::wstring Key);
  static std::wstring DecryptPassword(const std::wstring & Password, std::wstring Key);
  static std::wstring StronglyRecryptPassword(const std::wstring & Password, std::wstring Key);

  // __property std::wstring InternalStorageKey = { read = GetInternalStorageKey };
  std::wstring GetInternalStorageKey();

public:
  TSessionData(std::wstring aName);
  void Default();
  void NonPersistant();
  void Load(THierarchicalStorage * Storage);
  void Save(THierarchicalStorage * Storage, bool PuttyExport,
    const TSessionData * Default = NULL);
  void SaveRecryptedPasswords(THierarchicalStorage * Storage);
  void RecryptPasswords();
  bool HasAnyPassword();
  void Remove();
  virtual void Assign(TPersistent * Source);
  bool ParseUrl(std::wstring Url, TOptions * Options,
    TStoredSessionList * StoredSessions, bool & DefaultsOnly,
    std::wstring * FileName, bool * AProtocolDefined);
  bool ParseOptions(TOptions * Options);
  void ConfigureTunnel(int PortNumber);
  void RollbackTunnel();
  void ExpandEnvironmentVariables();
  static void ValidatePath(const std::wstring Path);
  static void ValidateName(const std::wstring Name);

  // __property std::wstring HostName  = { read=FHostName, write=SetHostName };
  std::wstring GetHostName() { return FHostName; }
  // __property int PortNumber  = { read=FPortNumber, write=SetPortNumber };
  int GetPortNumber() { return FPortNumber; }
  // __property std::wstring UserName  = { read=FUserName, write=SetUserName };
  std::wstring GetUserName() { return FUserName; }
  // __property std::wstring Password  = { read=GetPassword, write=SetPassword };
  void SetPassword(std::wstring value);
  std::wstring GetPassword();
  // __property bool Passwordless = { read=FPasswordless, write=SetPasswordless };
  bool GetPasswordless() { return FPasswordless; }
  // __property int PingInterval  = { read=FPingInterval, write=SetPingInterval };
  int GetPingInterval() { return FPingInterval; }
  // __property bool TryAgent  = { read=FTryAgent, write=SetTryAgent };
  bool GetTryAgent() { return FTryAgent; }
  // __property bool AgentFwd  = { read=FAgentFwd, write=SetAgentFwd };
  bool GetAgentFwd() { return FAgentFwd; }
  // __property std::wstring ListingCommand = { read = FListingCommand, write = SetListingCommand };
  std::wstring GetListingCommand() { return FListingCommand; }
  // __property bool AuthTIS  = { read=FAuthTIS, write=SetAuthTIS };
  bool GetAuthTIS() { return FAuthTIS; }
  // __property bool AuthKI  = { read=FAuthKI, write=SetAuthKI };
  bool GetAuthKI() { return FAuthKI; }
  // __property bool AuthKIPassword  = { read=FAuthKIPassword, write=SetAuthKIPassword };
  bool GetAuthKIPassword() { return FAuthKIPassword; }
  // __property bool AuthGSSAPI  = { read=FAuthGSSAPI, write=SetAuthGSSAPI };
  bool GetAuthGSSAPI() { return FAuthGSSAPI; }
  // __property bool GSSAPIFwdTGT = { read=FGSSAPIFwdTGT, write=SetGSSAPIFwdTGT };
  bool GetGSSAPIFwdTGT() { return FGSSAPIFwdTGT; }
  // __property std::wstring GSSAPIServerRealm = { read=FGSSAPIServerRealm, write=SetGSSAPIServerRealm };
  std::wstring GetGSSAPIServerRealm() { return FGSSAPIServerRealm; }
  // __property bool ChangeUsername  = { read=FChangeUsername, write=SetChangeUsername };
  bool GetChangeUsername() { return FChangeUsername; }
  // __property bool Compression  = { read=FCompression, write=SetCompression };
  bool GetCompression() { return FCompression; }
  // __property TSshProt SshProt  = { read=FSshProt, write=SetSshProt };
  TSshProt GetSshProt() { return FSshProt; }
  // __property bool UsesSsh = { read = GetUsesSsh };
  bool GetUsesSsh();
  // __property bool Ssh2DES  = { read=FSsh2DES, write=SetSsh2DES };
  bool GetSsh2DES() { return FSsh2DES; }
  // __property bool SshNoUserAuth  = { read=FSshNoUserAuth, write=SetSshNoUserAuth };
  bool GetSshNoUserAuth() { return FSshNoUserAuth; }
  // __property TCipher Cipher[int Index] = { read=GetCipher, write=SetCipher };
  void SetCipher(int Index, TCipher value);
  TCipher GetCipher(int Index) const;
  // __property TKex Kex[int Index] = { read=GetKex, write=SetKex };
  void SetKex(int Index, TKex value);
  TKex GetKex(int Index) const;
  // __property std::wstring PublicKeyFile  = { read=FPublicKeyFile, write=SetPublicKeyFile };
  std::wstring GetPublicKeyFile() { return FPublicKeyFile; }
  // __property TProtocol Protocol  = { read=FProtocol, write=SetProtocol };
  TProtocol GetProtocol() { return FProtocol; }
  // __property std::wstring ProtocolStr  = { read=GetProtocolStr, write=SetProtocolStr };
  void SetProtocolStr(std::wstring value);
  std::wstring GetProtocolStr() const;
  // __property TFSProtocol FSProtocol  = { read=FFSProtocol, write=SetFSProtocol  };
  TFSProtocol GetFSProtocol() { return FFSProtocol; }
  // __property std::wstring FSProtocolStr  = { read=GetFSProtocolStr };
  std::wstring GetFSProtocolStr();
  // __property bool Modified  = { read=FModified, write=FModified };
  bool GetModified() { return FModified; }
  void SetModified(bool value) { FModified = value; }
  // __property bool CanLogin  = { read=GetCanLogin };
  bool GetCanLogin();
  // __property bool ClearAliases = { read = FClearAliases, write = SetClearAliases };
  bool GetClearAliases() { return FClearAliases; }
  // __property TDateTime PingIntervalDT = { read = GetPingIntervalDT, write = SetPingIntervalDT };
  void SetPingIntervalDT(TDateTime value);
  TDateTime GetPingIntervalDT();
  // __property TDateTime TimeDifference = { read = FTimeDifference, write = SetTimeDifference };
  TDateTime GetTimeDifference() { return FTimeDifference; }
  // __property TPingType PingType = { read = FPingType, write = SetPingType };
  TPingType GetPingType() { return FPingType; }
  // __property std::wstring SessionName  = { read=GetSessionName };
  std::wstring GetSessionName();
  // __property std::wstring DefaultSessionName  = { read=GetDefaultSessionName };
  std::wstring GetDefaultSessionName();
  // __property std::wstring SessionUrl  = { read=GetSessionUrl };
  std::wstring GetSessionUrl();
  // __property std::wstring LocalDirectory  = { read=FLocalDirectory, write=SetLocalDirectory };
  std::wstring GetLocalDirectory() { return FLocalDirectory; }
  // __property std::wstring RemoteDirectory  = { read=FRemoteDirectory, write=SetRemoteDirectory };
  std::wstring GetRemoteDirectory() { return FRemoteDirectory; }
  void SetRemoteDirectory(std::wstring value);
    // __property bool UpdateDirectories = { read=FUpdateDirectories, write=SetUpdateDirectories };
  bool GetUpdateDirectories() { return FUpdateDirectories; }
  // __property bool CacheDirectories = { read=FCacheDirectories, write=SetCacheDirectories };
  bool GetCacheDirectories() { return FCacheDirectories; }
  // __property bool CacheDirectoryChanges = { read=FCacheDirectoryChanges, write=SetCacheDirectoryChanges };
  bool GetCacheDirectoryChanges() { return FCacheDirectoryChanges; }
  // __property bool PreserveDirectoryChanges = { read=FPreserveDirectoryChanges, write=SetPreserveDirectoryChanges };
  bool GetPreserveDirectoryChanges() { return FPreserveDirectoryChanges; }
  // __property bool LockInHome = { read=FLockInHome, write=SetLockInHome };
  bool GetLockInHome() { return FLockInHome; }
  // __property bool Special = { read=FSpecial, write=SetSpecial };
  bool GetSpecial() { return FSpecial; }
  // __property bool Selected  = { read=FSelected, write=FSelected };
  bool GetSelected() { return FSelected; }
  // __property std::wstring InfoTip  = { read=GetInfoTip };
  std::wstring GetInfoTip();
  // __property bool DefaultShell = { read = GetDefaultShell, write = SetDefaultShell };
  bool GetDefaultShell();
  void SetDefaultShell(bool value);
  // __property bool DetectReturnVar = { read = GetDetectReturnVar, write = SetDetectReturnVar };
  void SetDetectReturnVar(bool value);
  bool GetDetectReturnVar();
  // __property TEOLType EOLType = { read = FEOLType, write = SetEOLType };
  TEOLType GetEOLType() { return FEOLType; }
  // __property bool LookupUserGroups = { read = FLookupUserGroups, write = SetLookupUserGroups };
  bool GetLookupUserGroups() { return FLookupUserGroups; }
  // __property std::wstring ReturnVar = { read = FReturnVar, write = SetReturnVar };
  std::wstring GetReturnVar() { return FReturnVar; }
  // __property bool Scp1Compatibility = { read = FScp1Compatibility, write = SetScp1Compatibility };
  bool GetScp1Compatibility() { return FScp1Compatibility; }
  // __property std::wstring Shell = { read = FShell, write = SetShell };
  std::wstring GetShell() { return FShell; }
  // __property std::wstring SftpServer = { read = FSftpServer, write = SetSftpServer };
  std::wstring GetSftpServer() { return FSftpServer; }
  // __property int Timeout = { read = FTimeout, write = SetTimeout };
  int GetTimeout() { return FTimeout; }
  // __property TDateTime TimeoutDT = { read = GetTimeoutDT };
  TDateTime GetTimeoutDT();
  // __property bool UnsetNationalVars = { read = FUnsetNationalVars, write = SetUnsetNationalVars };
  bool GetUnsetNationalVars() { return FUnsetNationalVars; }
  // __property bool IgnoreLsWarnings  = { read=FIgnoreLsWarnings, write=SetIgnoreLsWarnings };
  bool GetIgnoreLsWarnings() { return FIgnoreLsWarnings; }
  // __property bool TcpNoDelay  = { read=FTcpNoDelay, write=SetTcpNoDelay };
  bool GetTcpNoDelay() { return FTcpNoDelay; }
  // __property std::wstring SshProtStr  = { read=GetSshProtStr };
  std::wstring GetSshProtStr();
  // __property std::wstring CipherList  = { read=GetCipherList, write=SetCipherList };
  void SetCipherList(std::wstring value);
  std::wstring GetCipherList() const;
  // __property std::wstring KexList  = { read=GetKexList, write=SetKexList };
  void SetKexList(std::wstring value);
  std::wstring GetKexList() const;
  // __property TProxyMethod ProxyMethod  = { read=FProxyMethod, write=SetProxyMethod };
  TProxyMethod GetProxyMethod() { return FProxyMethod; }
  // __property std::wstring ProxyHost  = { read=FProxyHost, write=SetProxyHost };
  std::wstring GetProxyHost() { return FProxyHost; }
  // __property int ProxyPort  = { read=FProxyPort, write=SetProxyPort };
  int GetProxyPort() { return FProxyPort; }
  // __property std::wstring ProxyUsername  = { read=FProxyUsername, write=SetProxyUsername };
  std::wstring GetProxyUsername() { return FProxyUsername; }
  // __property std::wstring ProxyPassword  = { read=GetProxyPassword, write=SetProxyPassword };
  std::wstring GetProxyPassword() const;
  void SetProxyPassword(std::wstring value);
  // __property std::wstring ProxyTelnetCommand  = { read=FProxyTelnetCommand, write=SetProxyTelnetCommand };
  std::wstring GetProxyTelnetCommand() { return FProxyTelnetCommand; }
  // __property std::wstring ProxyLocalCommand  = { read=FProxyLocalCommand, write=SetProxyLocalCommand };
  std::wstring GetProxyLocalCommand() { return FProxyLocalCommand; }
  // __property TAutoSwitch ProxyDNS  = { read=FProxyDNS, write=SetProxyDNS };
  TAutoSwitch GetProxyDNS() { return FProxyDNS; }
  // __property bool ProxyLocalhost  = { read=FProxyLocalhost, write=SetProxyLocalhost };
  bool GetProxyLocalhost() { return FProxyLocalhost; }
  // __property int FtpProxyLogonType  = { read=FFtpProxyLogonType, write=SetFtpProxyLogonType };
  int GetFtpProxyLogonType() { return FFtpProxyLogonType; }
  // __property TAutoSwitch Bug[TSshBug Bug]  = { read=GetBug, write=SetBug };
  void SetBug(TSshBug Bug, TAutoSwitch value);
  TAutoSwitch GetBug(TSshBug Bug) const;
  // __property std::wstring CustomParam1 = { read = FCustomParam1, write = SetCustomParam1 };
  std::wstring GetCustomParam1() { return FCustomParam1; }
  // __property std::wstring CustomParam2 = { read = FCustomParam2, write = SetCustomParam2 };
  std::wstring GetCustomParam2() { return FCustomParam2; }
  // __property std::wstring SessionKey = { read = GetSessionKey };
  std::wstring GetSessionKey();
  // __property bool ResolveSymlinks = { read = FResolveSymlinks, write = SetResolveSymlinks };
  bool GetResolveSymlinks() { return FResolveSymlinks; }
  // __property int SFTPDownloadQueue = { read = FSFTPDownloadQueue, write = SetSFTPDownloadQueue };
  // __property int SFTGetPDownloadQueue = { read = FSFTPDownloadQueue, write = SetSFTPDownloadQueue };
  int GetSFTGetPDownloadQueue() { return FSFTPDownloadQueue; }
  void SetSFTPDownloadQueue(int value);
  // __property int SFTPUploadQueue = { read = FSFTPUploadQueue, write = SetSFTPUploadQueue };
  int GetSFTPUploadQueue() { return FSFTPUploadQueue; }
  // __property int SFTPListingQueue = { read = FSFTPListingQueue, write = SetSFTPListingQueue };
  int GetSFTPListingQueue() { return FSFTPListingQueue; }
  // __property int SFTPMaxVersion = { read = FSFTPMaxVersion, write = SetSFTPMaxVersion };
  int GetSFTPMaxVersion() { return FSFTPMaxVersion; }
  // __property unsigned long SFTPMaxPacketSize = { read = FSFTPMaxPacketSize, write = SetSFTPMaxPacketSize };
  unsigned long GetSFTPMaxPacketSize() { return FSFTPMaxPacketSize; }
  // __property TAutoSwitch SFTPBug[TSftpBug Bug]  = { read=GetSFTPBug, write=SetSFTPBug };
  void SetSFTPBug(TSftpBug Bug, TAutoSwitch value);
  TAutoSwitch GetSFTPBug(TSftpBug Bug) const;
  // __property TAutoSwitch SCPLsFullTime = { read = FSCPLsFullTime, write = SetSCPLsFullTime };
  TAutoSwitch GetSCPLsFullTime() { return FSCPLsFullTime; }
  // __property TAutoSwitch FtpListAll = { read = FFtpListAll, write = SetFtpListAll };
  TAutoSwitch GetFtpListAll() { return FFtpListAll; }
  // __property TDSTMode DSTMode = { read = FDSTMode, write = SetDSTMode };
  TDSTMode GetDSTMode() { return FDSTMode; }
  // __property bool DeleteToRecycleBin = { read = FDeleteToRecycleBin, write = SetDeleteToRecycleBin };
  bool GetDeleteToRecycleBin() { return FDeleteToRecycleBin; }
  // __property bool OverwrittenToRecycleBin = { read = FOverwrittenToRecycleBin, write = SetOverwrittenToRecycleBin };
  bool GetOverwrittenToRecycleBin() { return FOverwrittenToRecycleBin; }
  // __property std::wstring RecycleBinPath = { read = FRecycleBinPath, write = SetRecycleBinPath };
  std::wstring GetRecycleBinPath() { return FRecycleBinPath; }
  // __property std::wstring PostLoginCommands = { read = FPostLoginCommands, write = SetPostLoginCommands };
  std::wstring GetPostLoginCommands() { return FPostLoginCommands; }
  // __property TAddressFamily AddressFamily = { read = FAddressFamily, write = SetAddressFamily };
  TAddressFamily GetAddressFamily() { return FAddressFamily; }
  // __property std::wstring RekeyData = { read = FRekeyData, write = SetRekeyData };
  std::wstring GetRekeyData() { return FRekeyData; }
  // __property unsigned int RekeyTime = { read = FRekeyTime, write = SetRekeyTime };
  unsigned int GetRekeyTime() { return FRekeyTime; }
  // __property int Color = { read = FColor, write = SetColor };
  int GetColor() { return FColor; }
  // __property bool Tunnel = { read = FTunnel, write = SetTunnel };
  bool GetTunnel() { return FTunnel; }
  void SetTunnel(bool value);
  // __property std::wstring TunnelHostName = { read = FTunnelHostName, write = SetTunnelHostName };
  std::wstring GetTunnelHostName() { return FTunnelHostName; }
  // __property int TunnelPortNumber = { read = FTunnelPortNumber, write = SetTunnelPortNumber };
  int GetTunnelPortNumber() { return FTunnelPortNumber; }
  // __property std::wstring TunnelUserName = { read = FTunnelUserName, write = SetTunnelUserName };
  std::wstring GetTunnelUserName() { return FTunnelUserName; }
  // __property std::wstring TunnelPassword = { read = GetTunnelPassword, write = SetTunnelPassword };
  void SetTunnelPassword(std::wstring value);
  std::wstring GetTunnelPassword();
  // __property std::wstring TunnelPublicKeyFile = { read = FTunnelPublicKeyFile, write = SetTunnelPublicKeyFile };
  std::wstring GetTunnelPublicKeyFile() { return FTunnelPublicKeyFile; }
  // __property bool TunnelAutoassignLocalPortNumber = { read = GetTunnelAutoassignLocalPortNumber };
  bool GetTunnelAutoassignLocalPortNumber();
  // __property int TunnelLocalPortNumber = { read = FTunnelLocalPortNumber, write = SetTunnelLocalPortNumber };
  int GetTunnelLocalPortNumber() { return FTunnelLocalPortNumber; }
  // __property std::wstring TunnelPortFwd = { read = FTunnelPortFwd, write = SetTunnelPortFwd };
  std::wstring GetTunnelPortFwd() { return FTunnelPortFwd; }
  void SetTunnelPortFwd(std::wstring value);
  // __property bool FtpPasvMode = { read = FFtpPasvMode, write = SetFtpPasvMode };
  bool GetFtpPasvMode() { return FFtpPasvMode; }
  // __property bool FtpForcePasvIp = { read = FFtpForcePasvIp, write = SetFtpForcePasvIp };
  bool GetFtpForcePasvIp() { return FFtpForcePasvIp; }
  // __property std::wstring FtpAccount = { read = FFtpAccount, write = SetFtpAccount };
  std::wstring GetFtpAccount() { return FFtpAccount; }
  // __property int FtpPingInterval  = { read=FFtpPingInterval, write=SetFtpPingInterval };
  int GetFtpPingInterval() { return FFtpPingInterval; }
  // __property TDateTime FtpPingIntervalDT  = { read=GetFtpPingIntervalDT };
  TDateTime GetFtpPingIntervalDT();
  // __property TPingType FtpPingType = { read = FFtpPingType, write = SetFtpPingType };
  TPingType GetFtpPingType() { return FFtpPingType; }
  // __property TFtps Ftps = { read = FFtps, write = SetFtps };
  TFtps GetFtps() { return FFtps; }
  // __property TAutoSwitch NotUtf = { read = FNotUtf, write = SetNotUtf };
  TAutoSwitch GetNotUtf() { return FNotUtf; }
  // __property std::wstring HostKey = { read = FHostKey, write = SetHostKey };
  std::wstring GetHostKey() { return FHostKey; }
  // __property std::wstring StorageKey = { read = GetStorageKey };
  std::wstring GetStorageKey();
  // __property std::wstring OrigHostName = { read = FOrigHostName };
  std::wstring GetOrigHostName() { return FOrigHostName; }
  // __property int OrigPortNumber = { read = FOrigPortNumber };
  int GetOrigPortNumber() { return FOrigPortNumber; }
  // __property std::wstring LocalName = { read = GetLocalName };
  std::wstring GetLocalName();
  // __property std::wstring Source = { read = GetSource };
  std::wstring GetSource();

  void SetHostName(std::wstring value);
  void SetPortNumber(int value);
  void SetUserName(std::wstring value);
  void SetPasswordless(bool value);
  void SetPingInterval(int value);
  void SetTryAgent(bool value);
  void SetAgentFwd(bool value);
  void SetAuthTIS(bool value);
  void SetAuthKI(bool value);
  void SetAuthKIPassword(bool value);
  void SetAuthGSSAPI(bool value);
  void SetGSSAPIFwdTGT(bool value);
  void SetGSSAPIServerRealm(std::wstring value);
  void SetChangeUsername(bool value);
  void SetCompression(bool value);
  void SetSshProt(TSshProt value);
  void SetSsh2DES(bool value);
  void SetSshNoUserAuth(bool value);
  void SetPublicKeyFile(std::wstring value);

  void SetTimeDifference(TDateTime value);
  void SetPingType(TPingType value);
  void SetProtocol(TProtocol value);
  void SetFSProtocol(TFSProtocol value);
  void SetLocalDirectory(std::wstring value);
  void SetUpdateDirectories(bool value);
  void SetCacheDirectories(bool value);
  void SetCacheDirectoryChanges(bool value);
  void SetPreserveDirectoryChanges(bool value);
  void SetLockInHome(bool value);
  void SetSpecial(bool value);
  void SetListingCommand(std::wstring value);
  void SetClearAliases(bool value);
  void SetEOLType(TEOLType value);
  void SetLookupUserGroups(bool value);
  void SetReturnVar(std::wstring value);
  void SetScp1Compatibility(bool value);
  void SetShell(std::wstring value);
  void SetSftpServer(std::wstring value);
  void SetTimeout(int value);
  void SetUnsetNationalVars(bool value);
  void SetIgnoreLsWarnings(bool value);
  void SetTcpNoDelay(bool value);
  void SetProxyMethod(TProxyMethod value);
  void SetProxyHost(std::wstring value);
  void SetProxyPort(int value);
  void SetProxyUsername(std::wstring value);
  void SetProxyTelnetCommand(std::wstring value);
  void SetProxyLocalCommand(std::wstring value);
  void SetProxyDNS(TAutoSwitch value);
  void SetProxyLocalhost(bool value);
  void SetFtpProxyLogonType(int value);
  void SetCustomParam1(std::wstring value);
  void SetCustomParam2(std::wstring value);
  void SetResolveSymlinks(bool value);
  void SetSFTPUploadQueue(int value);
  void SetSFTPListingQueue(int value);
  void SetSFTPMaxVersion(int value);
  void SetSFTPMaxPacketSize(unsigned long value);
  void SetSCPLsFullTime(TAutoSwitch value);
  void SetFtpListAll(TAutoSwitch value);
  void SetDSTMode(TDSTMode value);
  void SetDeleteToRecycleBin(bool value);
  void SetOverwrittenToRecycleBin(bool value);
  void SetRecycleBinPath(std::wstring value);
  void SetPostLoginCommands(std::wstring value);
  void SetAddressFamily(TAddressFamily value);
  void SetRekeyData(std::wstring value);
  void SetRekeyTime(unsigned int value);
  void SetColor(int value);
  void SetTunnelHostName(std::wstring value);
  void SetTunnelPortNumber(int value);
  void SetTunnelUserName(std::wstring value);
  void SetTunnelPublicKeyFile(std::wstring value);
  void SetTunnelLocalPortNumber(int value);
  void SetFtpPasvMode(bool value);
  void SetFtpForcePasvIp(bool value);
  void SetFtpAccount(std::wstring value);
  void SetFtpPingInterval(int value);
  void SetFtpPingType(TPingType value);
  void SetFtps(TFtps value);
  void SetNotUtf(TAutoSwitch value);
  void SetHostKey(std::wstring value);
};
//---------------------------------------------------------------------------
class TStoredSessionList : public TNamedObjectList
{
public:
  TStoredSessionList(bool aReadOnly = false);
  void Load(std::wstring aKey, bool UseDefaults);
  void Load();
  void Save(bool All, bool Explicit);
  void Saved();
  void Export(const std::wstring FileName);
  void Load(THierarchicalStorage * Storage, bool AsModified = false,
    bool UseDefaults = false);
  void Save(THierarchicalStorage * Storage, bool All = false);
  void SelectAll(bool Select);
  void Import(TStoredSessionList * From, bool OnlySelected);
  void RecryptPasswords();
  void SelectSessionsToImport(TStoredSessionList * Dest, bool SSHOnly);
  void Cleanup();
  int IndexOf(TSessionData * Data);
  TSessionData * NewSession(std::wstring SessionName, TSessionData * Session);
  TSessionData * ParseUrl(std::wstring Url, TOptions * Options, bool & DefaultsOnly,
    std::wstring * FileName = NULL, bool * ProtocolDefined = NULL);
  virtual ~TStoredSessionList();
  // __property TSessionData * Sessions[int Index]  = { read=AtSession };
  TSessionData *GetSession(int Index) { return (TSessionData*)AtObject(Index); }
  // __property TSessionData * DefaultSettings  = { read=FDefaultSettings, write=SetDefaultSettings };
  TSessionData *GetDefaultSettings() { return FDefaultSettings; }
  void SetDefaultSettings(TSessionData * value);

  static void ImportHostKeys(const std::wstring TargetKey,
    const std::wstring SourceKey, TStoredSessionList * Sessions,
    bool OnlySelected);

private:
  TSessionData * FDefaultSettings;
  bool FReadOnly;
  void DoSave(THierarchicalStorage * Storage, bool All, bool RecryptPasswordOnly);
  void DoSave(bool All, bool Explicit, bool RecryptPasswordOnly);
  void DoSave(THierarchicalStorage * Storage,
    TSessionData * Data, bool All, bool RecryptPasswordOnly,
    TSessionData * FactoryDefaults);
};
//---------------------------------------------------------------------------
#endif
