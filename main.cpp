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
        Call *call = new MyCall(*this, iprm.callId);

        CallOpParam op;
        op.statusCode = PJSIP_SC_RINGING;
        call->answer(op);

        delete call;
    }
};


int main() {
    Endpoint ep;
    ep.libCreate();

    // Init endpoint
    EpConfig ep_cfg;
    ep.libInit(ep_cfg);

    TransportConfig tcfg;
    tcfg.port = 5060;
    try {
        ep.transportCreate(PJSIP_TRANSPORT_UDP, tcfg);
    } catch (Error &err) {
        std::cout << err.info() << std::endl;
        return 1;
    }

    // Start the library
    ep.libStart();
    std::cout << "*** PJSUA2 STARTED ***" << std::endl;

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
    AuthCredInfo cred("digest", "*", sip_username, 0, sip_password);
    acfg.sipConfig.authCreds.push_back(cred);

    MyAccount *acc = new MyAccount;
    acc->create(acfg);

    pj_thread_sleep(100000);

    delete acc;

    return 0;
}
