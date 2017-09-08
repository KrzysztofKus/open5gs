#define TRACE_MODULE _emm_sm

#include "core_debug.h"

#include "nas_message.h"
#include "fd_lib.h"
#include "s6a_message.h"

#include "mme_event.h"
#include "mme_kdf.h"
#include "s1ap_handler.h"
#include "mme_fd_path.h"
#include "emm_handler.h"
#include "emm_build.h"
#include "esm_handler.h"
#include "nas_path.h"
#include "s1ap_path.h"
#include "mme_gtp_path.h"

void emm_state_initial(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");

    mme_sm_trace(3, e);

    FSM_TRAN(s, &emm_state_detached);
}

void emm_state_final(fsm_t *s, event_t *e)
{
    d_assert(s, return, "Null param");

    mme_sm_trace(3, e);
}

void emm_state_detached(fsm_t *s, event_t *e)
{
    mme_ue_t *mme_ue = NULL;

    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    mme_sm_trace(3, e);

    mme_ue = mme_ue_find(event_get_param1(e));
    d_assert(mme_ue, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        case MME_EVT_EMM_MESSAGE:
        {
            nas_message_t *message = (nas_message_t *)event_get_param4(e);
            d_assert(message, break, "Null param");

            switch(message->emm.h.message_type)
            {
                case NAS_ATTACH_REQUEST:
                {
                    emm_handle_attach_request(
                            mme_ue, &message->emm.attach_request);
                    break;
                }
            }
            break;
        }
        default:
        {
            d_error("Unknown event %s", mme_event_get_name(e));
            break;
        }
    }
}

void emm_state_identity(fsm_t *s, event_t *e)
{
    status_t rv;
    mme_ue_t *mme_ue = NULL;

    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    mme_sm_trace(3, e);

    mme_ue = mme_ue_find(event_get_param1(e));
    d_assert(mme_ue, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            pkbuf_t *emmbuf = NULL;

            rv = emm_build_identity_request(&emmbuf, mme_ue);
            d_assert(rv == CORE_OK && emmbuf, break, "emm build error");

            rv = nas_send_to_downlink_nas_transport(mme_ue, emmbuf);
            d_assert(rv == CORE_OK && emmbuf, break, "emm send error");

            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        case MME_EVT_EMM_MESSAGE:
        {
            nas_message_t *message = (nas_message_t *)event_get_param4(e);
            d_assert(message, break, "Null param");

            switch(message->emm.h.message_type)
            {
                case NAS_IDENTITY_RESPONSE:
                {
                    emm_handle_identity_response(mme_ue,
                            &message->emm.identity_response);
                    break;
                }
                case NAS_EMM_STATUS:
                {
                    emm_handle_emm_status(mme_ue, &message->emm.emm_status);
                    FSM_TRAN(s, &emm_state_detached);
                    break;
                }
                default:
                {
                    d_warn("Unknown message(type:%d)", 
                            message->emm.h.message_type);
                    break;
                }
            }
            break;
        }
        default:
        {
            d_error("Unknown event %s", mme_event_get_name(e));
            break;
        }
    }
}

void emm_state_authentication(fsm_t *s, event_t *e)
{
    mme_ue_t *mme_ue = NULL;

    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    mme_sm_trace(3, e);

    mme_ue = mme_ue_find(event_get_param1(e));
    d_assert(mme_ue, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        case MME_EVT_EMM_MESSAGE:
        {
            nas_message_t *message = (nas_message_t *)event_get_param4(e);
            d_assert(message, break, "Null param");

            switch(message->emm.h.message_type)
            {
                case NAS_AUTHENTICATION_RESPONSE:
                {
                    emm_handle_authentication_response(
                            mme_ue, &message->emm.authentication_response);
                    FSM_TRAN(s, &emm_state_security_mode);
                    break;
                }
                case NAS_EMM_STATUS:
                {
                    emm_handle_emm_status(mme_ue, &message->emm.emm_status);
                    FSM_TRAN(s, &emm_state_detached);
                    break;
                }
                default:
                {
                    d_warn("Unknown message(type:%d)", 
                            message->emm.h.message_type);
                    break;
                }
            }
            break;
        }
        default:
        {
            d_error("Unknown event %s", mme_event_get_name(e));
            break;
        }
    }
}

void emm_state_security_mode(fsm_t *s, event_t *e)
{
    mme_ue_t *mme_ue = NULL;

    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    mme_sm_trace(3, e);

    mme_ue = mme_ue_find(event_get_param1(e));
    d_assert(mme_ue, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            status_t rv;
            pkbuf_t *emmbuf = NULL;

            rv = emm_build_security_mode_command(&emmbuf, mme_ue);
            d_assert(rv == CORE_OK && emmbuf, break, "emm build error");

            rv = nas_send_to_downlink_nas_transport(mme_ue, emmbuf);
            d_assert(rv == CORE_OK && emmbuf, break, "emm send error");

            d_trace(3, "[NAS] Security mode command : UE[%s] <-- EMM\n", 
                    mme_ue->imsi_bcd);
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        case MME_EVT_EMM_MESSAGE:
        {
            nas_message_t *message = (nas_message_t *)event_get_param4(e);
            d_assert(message, break, "Null param");

            switch(message->emm.h.message_type)
            {
                case NAS_SECURITY_MODE_COMPLETE:
                {
                    d_trace(3, "[NAS] Security mode complete : "
                            "UE[%s] --> EMM\n", mme_ue->imsi_bcd);

                    /* Update Kenb */
                    if (SECURITY_CONTEXT_IS_VALID(mme_ue))
                        mme_kdf_enb(mme_ue->kasme, mme_ue->ul_count.i32, 
                                mme_ue->kenb);

                    mme_s6a_send_ulr(mme_ue);
                    FSM_TRAN(s, &emm_state_default_esm);
                    break;
                }
                case NAS_EMM_STATUS:
                {
                    emm_handle_emm_status(mme_ue, &message->emm.emm_status);
                    FSM_TRAN(s, &emm_state_detached);
                    break;
                }
                default:
                {
                    d_warn("Unknown message(type:%d)", 
                            message->emm.h.message_type);
                    break;
                }
            }
            break;
        }
        default:
        {
            d_error("Unknown event %s", mme_event_get_name(e));
            break;
        }
    }
}

void emm_state_default_esm(fsm_t *s, event_t *e)
{
    mme_ue_t *mme_ue = NULL;

    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    mme_sm_trace(3, e);

    mme_ue = mme_ue_find(event_get_param1(e));
    d_assert(mme_ue, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        case MME_EVT_EMM_MESSAGE:
        {
            nas_message_t *message = (nas_message_t *)event_get_param4(e);
            d_assert(message, break, "Null param");

            switch(message->emm.h.message_type)
            {
                case NAS_ATTACH_COMPLETE:
                {
                    d_trace(3, "[NAS] Attach complete : UE[%s] --> EMM\n",
                            mme_ue->imsi_bcd);
                    emm_handle_attach_complete(
                            mme_ue, &message->emm.attach_complete);
                    FSM_TRAN(s, &emm_state_attached);
                    break;
                }
                case NAS_EMM_STATUS:
                {
                    emm_handle_emm_status(mme_ue, &message->emm.emm_status);
                    FSM_TRAN(s, &emm_state_detached);
                    break;
                }
                default:
                {
                    d_warn("Unknown message(type:%d)", 
                            message->emm.h.message_type);
                    break;
                }
            }
            break;
        }
        default:
        {
            d_error("Unknown event %s", mme_event_get_name(e));
            break;
        }
    }
}

void emm_state_attached(fsm_t *s, event_t *e)
{
    mme_ue_t *mme_ue = NULL;

    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    mme_sm_trace(3, e);

    mme_ue = mme_ue_find(event_get_param1(e));
    d_assert(mme_ue, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        case MME_EVT_EMM_MESSAGE:
        {
            nas_message_t *message = (nas_message_t *)event_get_param4(e);
            d_assert(message, break, "Null param");

            if (message->emm.h.security_header_type
                    == NAS_SECURITY_HEADER_FOR_SERVICE_REQUEST_MESSAGE)
            {
                emm_handle_service_request(
                        mme_ue, &message->emm.service_request);
                break;
            }

            switch(message->emm.h.message_type)
            {
                case NAS_ATTACH_REQUEST:
                {
                    emm_handle_attach_request(
                            mme_ue, &message->emm.attach_request);
                    break;
                }
                case NAS_EMM_STATUS:
                {
                    emm_handle_emm_status(mme_ue, &message->emm.emm_status);
                    FSM_TRAN(s, &emm_state_detached);
                    break;
                }
                case NAS_DETACH_REQUEST:
                {
                    emm_handle_detach_request(
                            mme_ue, &message->emm.detach_request_from_ue);
                    FSM_TRAN(s, &emm_state_detached);
                    break;
                }
                case NAS_TRACKING_AREA_UPDATE_REQUEST:
                {
                    emm_handle_tau_request(
                            mme_ue, &message->emm.tracking_area_update_request);
                    break;
                }
                case NAS_TRACKING_AREA_UPDATE_COMPLETE:
                {
                    status_t rv;
                    S1ap_Cause_t cause;
                    enb_ue_t *enb_ue = mme_ue->enb_ue;

                    d_assert(enb_ue, return, "Null param");

                    cause.present = S1ap_Cause_PR_nas;
                    cause.choice.nas = S1ap_CauseNas_normal_release;

                    rv = s1ap_send_ue_context_release_commmand(enb_ue, &cause);
                    d_assert(rv == CORE_OK, return, "s1ap send error");
                    break;
                }
                default:
                {
                    d_warn("Unknown message(type:%d)", 
                            message->emm.h.message_type);
                    break;
                }
            }
            break;
        }
        case MME_EVT_EMM_T3413:
        {
            if (mme_ue->max_paging_retry >= MAX_NUM_OF_PAGING)
            {
                /* Paging failed */
                d_warn("Paging to UE(%s) failed. Stop paging",
                        mme_ue->imsi_bcd);
                if (mme_ue->last_paging_msg)
                {
                    pkbuf_free(mme_ue->last_paging_msg);
                    mme_ue->last_paging_msg = NULL;
                }
                break;
            }

            mme_ue->max_paging_retry++;
            s1ap_handle_paging(mme_ue);
            /* Start T3413 */
            tm_start(mme_ue->t3413);

            break;
        }
        default:
        {
            d_error("Unknown event %s", mme_event_get_name(e));
            break;
        }
    }
}

void emm_state_exception(fsm_t *s, event_t *e)
{
    mme_ue_t *mme_ue = NULL;
    mme_sess_t *sess = NULL;
    mme_bearer_t *bearer = NULL;

    d_assert(s, return, "Null param");
    d_assert(e, return, "Null param");

    mme_sm_trace(3, e);

    bearer = mme_bearer_find(event_get_param1(e));
    d_assert(bearer, return, "Null param");
    sess = bearer->sess;
    d_assert(sess, return, "Null param");
    mme_ue = sess->mme_ue;
    d_assert(mme_ue, return, "Null param");

    switch (event_get(e))
    {
        case FSM_ENTRY_SIG:
        {
            break;
        }
        case FSM_EXIT_SIG:
        {
            break;
        }
        default:
        {
            d_error("Unknown event %s", mme_event_get_name(e));
            break;
        }
    }
}
