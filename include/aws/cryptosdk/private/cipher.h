/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may not use
 * this file except in compliance with the License. A copy of the License is
 * located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied. See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AWS_CRYPTOSDK_PRIVATE_CIPHER_H
#define AWS_CRYPTOSDK_PRIVATE_CIPHER_H

#include <aws/cryptosdk/cipher.h>

/**
 * Internal cryptographic helpers.
 * This header is not installed and is not a stable API.
 */

#define MAX_DATA_KEY_SIZE 32

struct data_key {
    uint8_t keybuf[MAX_DATA_KEY_SIZE];
};

struct content_key {
    uint8_t keybuf[MAX_DATA_KEY_SIZE];
};

/**
 * Derive the decryption key from the data key.
 * Depending on the algorithm ID, this either does a HKDF,
 * or a no-op copy of the key.
 */
int aws_cryptosdk_derive_key(
    const struct aws_cryptosdk_alg_properties *alg_props,
    struct content_key *content_key,
    const struct data_key *data_key,
    const uint8_t *message_id
);

/**
 * Verifies the header authentication tag.
 * Returns AWS_OP_SUCCESS if the tag is valid, raises AWS_CRYPTOSDK_ERR_BAD_CIPHERTEXT
 * if invalid.
 */
int aws_cryptosdk_verify_header(
    const struct aws_cryptosdk_alg_properties *alg_props,
    const struct content_key *content_key,
    const struct aws_byte_buf *authtag,
    const struct aws_byte_buf *header
);

/**
 * Computes the header authentication tag. The tag (and IV) is written to the authtag buffer.
 */
int aws_cryptosdk_sign_header(
    const struct aws_cryptosdk_alg_properties *alg_props,
    const struct content_key *content_key,
    const struct aws_byte_buf *authtag,
    const struct aws_byte_buf *header
);


enum aws_cryptosdk_frame_type {
    FRAME_TYPE_SINGLE,
    FRAME_TYPE_FRAME,
    FRAME_TYPE_FINAL
};

// TODO: Initialize the cipher once and reuse it
/**
 * Decrypts either the body of the message (for non-framed messages) or a single frame of the message.
 * Returns AWS_OP_SUCCESS if successful.
 */
int aws_cryptosdk_decrypt_body(
    const struct aws_cryptosdk_alg_properties *alg_props,
    struct aws_byte_cursor *out,
    const struct aws_byte_cursor *in,
    const uint8_t *message_id,
    uint32_t seqno,
    const uint8_t *iv,
    const struct content_key *key,
    const uint8_t *tag,
    int body_frame_type
);

/**
 * Encrypts either the body of the message (for non-framed messages) or a single frame of the message.
 * Returns AWS_OP_SUCCESS if successful.
 */
int aws_cryptosdk_encrypt_body(
    const struct aws_cryptosdk_alg_properties *alg_props,
    struct aws_byte_cursor *out,
    const struct aws_byte_cursor *in,
    const uint8_t *message_id,
    uint32_t seqno,
    uint8_t *iv, /* out */
    const struct content_key *key,
    uint8_t *tag, /* out */
    int body_frame_type
);


int aws_cryptosdk_genrandom(
    uint8_t *buf,
    size_t len
);

// TODO: Footer

/**
 * Assumes plain is an already allocated byte buffer with enough capacity to hold entire
 * decrypted plaintext. Makes no checks on capacity of plain and will overwrite the buffer
 * if called with too short a buffer, so be sure to allocate it to be big enough. Does NOT
 * assume that length of plain buffer is already set, and will set it to the length of plain
 * on a successful decrypt.
 *
 * Returns AWS_OP_ERR only in the case of an invalid key length or an OpenSSL error. Returns
 * AWS_OP_SUCCESS otherwise.
 *
 * On any unsuccessful AES-GCM decrypt, whether normal or due to OpenSSL error, the plain
 * buffer will be set to all zero bytes, and its length will be set to zero.
 *
 * Used by raw AES MK to do decryption of data keys. Uses the default IV length of 12 bytes
 * regardless of length specified by IV byte cursor.
 */
int aws_cryptosdk_aes_gcm_decrypt(struct aws_byte_buf * plain,
                                  const struct aws_byte_cursor cipher,
                                  const struct aws_byte_cursor tag,
                                  const struct aws_byte_cursor iv,
                                  const struct aws_byte_cursor aad,
                                  const struct aws_string * key);


#endif // AWS_CRYPTOSDK_PRIVATE_CIPHER_H
