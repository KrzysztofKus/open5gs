/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "S1AP-IEs"
 * 	found in "../support/s1ap-r16.1.0/36413-g10.asn"
 * 	`asn1c -pdu=all -fcompound-names -findirect-choice -fno-include-deps`
 */

#ifndef	_S1AP_CancelledCellinEAI_H_
#define	_S1AP_CancelledCellinEAI_H_


#include <asn_application.h>

/* Including external dependencies */
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct S1AP_CancelledCellinEAI_Item;

/* S1AP_CancelledCellinEAI */
typedef struct S1AP_CancelledCellinEAI {
	A_SEQUENCE_OF(struct S1AP_CancelledCellinEAI_Item) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} S1AP_CancelledCellinEAI_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_S1AP_CancelledCellinEAI;
extern asn_SET_OF_specifics_t asn_SPC_S1AP_CancelledCellinEAI_specs_1;
extern asn_TYPE_member_t asn_MBR_S1AP_CancelledCellinEAI_1[1];
extern asn_per_constraints_t asn_PER_type_S1AP_CancelledCellinEAI_constr_1;

#ifdef __cplusplus
}
#endif

#endif	/* _S1AP_CancelledCellinEAI_H_ */
#include <asn_internal.h>
