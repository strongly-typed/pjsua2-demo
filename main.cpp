#include <iostream>
#include <cstdlib>
#include <pjsua2.hpp>

using namespace pj;

class MyCall : public Call
{
public:
    MyCall(Account &acc, int call_id = PJSUA_INVALID_ID) : Call(acc, call_id)
    {}
};

class MyAccount : public Account {
public:
    virtual void onRegState(OnRegStateParam &prm) {
        AccountInfo ai = getInfo();
        std::cout << (ai.regIsActive ? "*** Register:" : "*** Unregister:")
                  << " code=" << prm.code << std::endl;
    }

    virtual void onIncomingCall(OnIncomingCallParam &iprm) {
        std::cout << "*** onIncomingCall callId=" << iprm.callId << std::endl;
        Call *call = new MyCall(*this, iprm.callId);

        CallInfo callInfo = call->getInfo();
        std::cout << "    accId = " << callInfo.accId << std::endl;
        std::cout << "    remAudioCount = " << callInfo.remAudioCount << std::endl;
        std::cout << "    remVideoCount = " << callInfo.remVideoCount << std::endl;

        CallOpParam op;
        op.opt.audioCount = 1;
        op.opt.videoCount = 0;
        op.statusCode = PJSIP_SC_RINGING;

        std::cout << "Sleep ... " << std::endl;
        pj_thread_sleep(1000);
        std::cout << "Answer ... " << std::endl;
        call->answer(op);

        delete call;
    }
};


int main() {
    Endpoint ep;
    ep.libCreate();

    // Get config from environment
    const char* sip_stun_uri = std::getenv("SIP_STUN_URI");
    if (not sip_stun_uri) { printf("SIP_STUN_URI not set by environment\n"); exit(-1); }

    // Init endpoint
    EpConfig ep_cfg;
    ep_cfg.uaConfig.stunServer.push_back(sip_stun_uri);
    ep_cfg.logConfig.level = 6;
    ep_cfg.logConfig.consoleLevel = 6;
    ep.libInit(ep_cfg);

    TransportConfig tcfg;
    tcfg.port = 5060;
    try {
        ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
        ep.transportCreate(PJSIP_TRANSPORT_TCP, tcfg);
    } catch (Error &err) {
        std::cout << err.info() << std::endl;
        return 1;
    }

    /* Use Null Audio Device as main media clock. This is useful for improving
     * media clock (see also https://trac.pjsip.org/repos/wiki/FAQ#tx-timing)
     * especially when sound device clock is jittery.
     */
    ep.audDevManager().setNullDev();

    // Get credentials from environment
    const char* sip_id_uri = std::getenv("SIP_ID_URI");
    const char* sip_registrar_uri = std::getenv("SIP_REGISTRAR_URI");
    const char* sip_username = std::getenv("SIP_USERNAME");
    const char* sip_password = std::getenv("SIP_PASSOWRD");

    if (not sip_id_uri) { printf("SIP_ID_URI not set by environment\n"); exit(-1); }
    if (not sip_registrar_uri) { printf("SIP_REGISTRAR_URI not set by environment\n"); exit(-1); }
    if (not sip_username) { printf("SIP_USERNAME not set by environment\n"); exit(-1); }
    if (not sip_password) { printf("SIP_PASSOWRD not set by environment\n"); exit(-1); }

    // Configure
    AccountConfig acfg;
    acfg.idUri = sip_id_uri;
    acfg.regConfig.registrarUri = sip_registrar_uri;
    acfg.regConfig.retryIntervalSec = 300;
    acfg.regConfig.firstRetryIntervalSec = 60;
    AuthCredInfo cred("digest", "*", sip_username, 0, sip_password);
    acfg.sipConfig.authCreds.push_back(cred);
    acfg.natConfig.mediaStunUse = PJSUA_STUN_USE_DEFAULT;

    MyAccount *acc = new MyAccount;
    acc->create(acfg);

    // Start the library
    ep.libStart();
    std::cout << "*** PJSUA2 STARTED ***" << std::endl;

    pj_thread_sleep(100000);

    delete acc;

    ep.libDestroy();

    return 0;
}
