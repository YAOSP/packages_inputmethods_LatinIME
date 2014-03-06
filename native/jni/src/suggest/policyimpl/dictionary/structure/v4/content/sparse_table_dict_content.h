/*
 * Copyright (C) 2013, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LATINIME_SPARSE_TABLE_DICT_CONTENT_H
#define LATINIME_SPARSE_TABLE_DICT_CONTENT_H

#include "defines.h"
#include "suggest/policyimpl/dictionary/structure/v4/content/dict_content.h"
#include "suggest/policyimpl/dictionary/structure/v4/ver4_dict_constants.h"
#include "suggest/policyimpl/dictionary/utils/buffer_with_extendable_buffer.h"
#include "suggest/policyimpl/dictionary/utils/dict_file_writing_utils.h"
#include "suggest/policyimpl/dictionary/utils/mmapped_buffer.h"
#include "suggest/policyimpl/dictionary/utils/sparse_table.h"

namespace latinime {

// TODO: Support multiple contents.
class SparseTableDictContent : public DictContent {
 public:
    AK_FORCE_INLINE SparseTableDictContent(const char *const dictPath,
            const char *const lookupTableFileName, const char *const addressTableFileName,
            const char *const contentFileName, const bool isUpdatable,
            const int sparseTableBlockSize, const int sparseTableDataSize)
            : mLookupTableBuffer(
                      MmappedBuffer::openBuffer(dictPath, lookupTableFileName, isUpdatable)),
              mAddressTableBuffer(
                      MmappedBuffer::openBuffer(dictPath, addressTableFileName, isUpdatable)),
              mContentBuffer(
                      MmappedBuffer::openBuffer(dictPath, contentFileName, isUpdatable)),
              mExpandableLookupTableBuffer(
                      mLookupTableBuffer ? mLookupTableBuffer->getBuffer() : nullptr,
                      mLookupTableBuffer ? mLookupTableBuffer->getBufferSize() : 0,
                      BufferWithExtendableBuffer::DEFAULT_MAX_ADDITIONAL_BUFFER_SIZE),
              mExpandableAddressTableBuffer(
                      mAddressTableBuffer ? mAddressTableBuffer->getBuffer() : nullptr,
                      mAddressTableBuffer ? mAddressTableBuffer->getBufferSize() : 0,
                      BufferWithExtendableBuffer::DEFAULT_MAX_ADDITIONAL_BUFFER_SIZE),
              mExpandableContentBuffer(mContentBuffer ? mContentBuffer->getBuffer() : nullptr,
                      mContentBuffer ? mContentBuffer->getBufferSize() : 0,
                      BufferWithExtendableBuffer::DEFAULT_MAX_ADDITIONAL_BUFFER_SIZE),
              mAddressLookupTable(&mExpandableLookupTableBuffer, &mExpandableAddressTableBuffer,
                      sparseTableBlockSize, sparseTableDataSize),
              mIsValid(mLookupTableBuffer && mAddressTableBuffer && mContentBuffer) {}

    SparseTableDictContent(const int sparseTableBlockSize, const int sparseTableDataSize)
            : mLookupTableBuffer(), mAddressTableBuffer(), mContentBuffer(),
              mExpandableLookupTableBuffer(Ver4DictConstants::MAX_DICTIONARY_SIZE),
              mExpandableAddressTableBuffer(Ver4DictConstants::MAX_DICTIONARY_SIZE),
              mExpandableContentBuffer(Ver4DictConstants::MAX_DICTIONARY_SIZE),
              mAddressLookupTable(&mExpandableLookupTableBuffer, &mExpandableAddressTableBuffer,
                      sparseTableBlockSize, sparseTableDataSize), mIsValid(true) {}

    virtual ~SparseTableDictContent() {}

    virtual bool isValid() const {
        return mIsValid;
    }

    bool isNearSizeLimit() const {
        return mExpandableLookupTableBuffer.isNearSizeLimit()
                || mExpandableAddressTableBuffer.isNearSizeLimit()
                || mExpandableContentBuffer.isNearSizeLimit();
    }

 protected:
    SparseTable *getUpdatableAddressLookupTable() {
        return &mAddressLookupTable;
    }

    const SparseTable *getAddressLookupTable() const {
        return &mAddressLookupTable;
    }

    BufferWithExtendableBuffer *getWritableContentBuffer() {
        return &mExpandableContentBuffer;
    }

    const BufferWithExtendableBuffer *getContentBuffer() const {
        return &mExpandableContentBuffer;
    }

    bool flush(const char *const dictDirPath, const char *const lookupTableFileName,
            const char *const addressTableFileName, const char *const contentFileName) const;

 private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(SparseTableDictContent);

    const MmappedBuffer::MmappedBufferPtr mLookupTableBuffer;
    const MmappedBuffer::MmappedBufferPtr mAddressTableBuffer;
    const MmappedBuffer::MmappedBufferPtr mContentBuffer;
    BufferWithExtendableBuffer mExpandableLookupTableBuffer;
    BufferWithExtendableBuffer mExpandableAddressTableBuffer;
    BufferWithExtendableBuffer mExpandableContentBuffer;
    SparseTable mAddressLookupTable;
    const bool mIsValid;
};
} // namespace latinime
#endif /* LATINIME_SPARSE_TABLE_DICT_CONTENT_H */
