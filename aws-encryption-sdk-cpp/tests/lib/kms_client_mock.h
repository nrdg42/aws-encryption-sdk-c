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

#ifndef AWS_ENCRYPTION_SDK_KMS_CLIENT_MOCK_H
#define AWS_ENCRYPTION_SDK_KMS_CLIENT_MOCK_H

#include <aws/core/utils/Outcome.h>
#include <aws/kms/KMSClient.h>
#include <aws/kms/model/DecryptRequest.h>
#include <aws/kms/model/DecryptResult.h>
#include <aws/kms/model/EncryptRequest.h>
#include <aws/kms/model/EncryptResult.h>
#include <aws/kms/model/GenerateDataKeyRequest.h>
#include <aws/kms/model/GenerateDataKeyResult.h>
#include <deque>

#include <aws/cryptosdk/cpp/kms_keyring.h>

#include "exports.h"

namespace Aws {
namespace Cryptosdk {
namespace Testing {

namespace Model = Aws::KMS::Model;

/**
 * This class simulates a mock for KmsClient. No cpp testing framework is allowed at this time
 */
class TESTLIB_CPP_API KmsClientMock : public Aws::KMS::KMSClient {
   public:
    KmsClientMock();
    ~KmsClientMock();

    Model::EncryptOutcome Encrypt(const Model::EncryptRequest &request) const;
    void ExpectEncryptAccumulator(const Model::EncryptRequest &request, Model::EncryptOutcome encrypt_return);

    Model::DecryptOutcome Decrypt(const Model::DecryptRequest &request) const;
    void ExpectDecryptAccumulator(const Model::DecryptRequest &request, Model::DecryptOutcome decrypt_return);

    Model::GenerateDataKeyOutcome GenerateDataKey(const Model::GenerateDataKeyRequest &request) const;
    void ExpectGenerateDataKey(
        const Model::GenerateDataKeyRequest &request, Model::GenerateDataKeyOutcome generate_dk_return);

    void ExpectGrantTokens(const Aws::Vector<Aws::String> &grant_tokens);

    bool ExpectingOtherCalls();

   private:
    struct ExpectedEncryptValues {
        Model::EncryptRequest expected_enc_request;
        Model::EncryptOutcome encrypt_return;
    };
    mutable std::deque<ExpectedEncryptValues> expected_encrypt_values;

    struct ExpectedDecryptValues {
        Model::DecryptRequest expected_dec_request;
        Model::DecryptOutcome return_decrypt;
    };
    mutable std::deque<ExpectedDecryptValues> expected_decrypt_values;

    mutable bool expect_generate_dk;
    Model::GenerateDataKeyRequest expected_generate_dk_request;
    Model::GenerateDataKeyOutcome generate_dk_return;

    Aws::Vector<Aws::String> grant_tokens;
};

class TESTLIB_CPP_API KmsClientSupplierMock : public Aws::Cryptosdk::KmsKeyring::ClientSupplier {
   public:
    KmsClientSupplierMock()  = default;
    ~KmsClientSupplierMock() = default;
    std::shared_ptr<KMS::KMSClient> GetClient(const Aws::String &region, std::function<void()> &report_success);

    /**
     * Returns the KmsClientMock constructed for the given region if it's been
     * requested before (via GetClient), or an empty shared_ptr otherwise.
     */
    const std::shared_ptr<KmsClientMock> GetClientMock(const Aws::String &region) const;

    /**
     * Returns a read-only view of the internal map of KmsClientMock objects.
     */
    const Aws::Map<Aws::String, std::shared_ptr<KmsClientMock>> &GetClientMocksMap() const;

   private:
    mutable Aws::Map<Aws::String, std::shared_ptr<KmsClientMock>> kms_client_mocks;
};

}  // namespace Testing
}  // namespace Cryptosdk
}  // namespace Aws

#endif  // AWS_ENCRYPTION_SDK_KMS_CLIENT_MOCK_H
