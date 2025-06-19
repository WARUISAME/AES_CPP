#pragma once
#include <vector>
#include <cstdint>
#include <algorithm> // rotate
#include "State.h"
#include "Sbox.h"
#include "GFCPoly.h"
#include "GFPoly.h"

#include <stdexcept>
#include <random>

#include <immintrin.h> // AES-NI
#include <wmmintrin.h>
#include <immintrin.h>
// AES-NI���T�|�[�g����Ă��邩�ǂ������m�F���邽�߂̃w�b�_�[
#if defined(_MSC_VER)
    #include <intrin.h>
#elif defined(__GNUC__)
    #include <cpuid.h>
#endif

#include <cstring>

// https://nvlpubs.nist.gov/nistpubs/FIPS/NIST.FIPS.197.pdf

class AES {
public:
    AES(const std::vector<uint8_t>& cipherKey, const bool aesniflag);

    // Encrypt using CBC mode
    std::vector<uint8_t> encrypt_cbc(const std::vector<uint8_t>& plain_text);

    // Decrypt using CBC mode
    std::vector<uint8_t> decrypt_cbc(const std::vector<uint8_t>& cipher_text);

private:
    // --- �\�t�g�E�F�A�����̊֐� ---
    
    // State�̊e�o�C�g��S-box�Œu��
    State subBytes(const State& s);

    // State�̊e�o�C�g�����ɃV�t�g
    State shiftRows(const State& s);

    // State�̊e��ɑ΂��đ�����������s��
    State mixColumns(const State& s);

    // ubBytes�̋t�ϊ�
    State invSubBytes(const State& s);

    // shiftRows�̋t�ϊ�
    State invShiftRows(const State& s);

    // mixColumns�̋t�ϊ�
    State invMixColumns(const State& s);

    // State��RoundKey��XOR���Z
    State addRoundKey(const State& st, const std::vector<uint32_t>& keyScheduleWords);
    
    // 32bit�̃��[�h�̊e�o�C�g��S-box�u��
    uint32_t subWord(uint32_t word);

    // 32bit�̃��[�h��1�o�C�g���ɃV�t�g
    uint32_t rotWord(uint32_t word);

    // �Í����L�[����e���E���h�L�[�𐶐�
    std::vector<uint32_t> keyExpansion();

    std::vector<uint8_t> cipher(const std::vector<uint8_t>& inputBytes);

    std::vector<uint8_t> invCipher(const std::vector<uint8_t>& inputBytes);

    // �Í���
    std::vector<uint8_t> encrypt(const std::vector<uint8_t>& input_bytes);

    // ������
    std::vector<uint8_t> decrypt(const std::vector<uint8_t>& cipher_text);

    // --- AES-NI�����̊֐� ---
    
    // AES-NI���g�p����CBC���[�h�ňÍ���
    std::vector<uint8_t> encryptAESNI_cbc(const std::vector<uint8_t>& plain_text);
    // AES-NI���g�p����CBC���[�h�ŕ�����
    std::vector<uint8_t> decryptAESNI_cbc(const std::vector<uint8_t>& cipher_text);

    __m128i encrypt_block(__m128i block) const;
    __m128i decrypt_block(__m128i block) const;

    // AES-128 �̃L�[�g���p
    inline __m128i AES_128_ASSIST_IMPL(__m128i temp1, __m128i temp2);
    // AES-192 �̃L�[�g���p
    inline void AES_192_ASSIST(__m128i* temp1, __m128i* temp2, __m128i* temp3);

    // --- ���ʂ̊֐� ---
    
    // Generate a random IV (Initialization Vector)
    std::vector<uint8_t> generate_iv();

    // XOR two vectors
    std::vector<uint8_t> xor_vectors(const std::vector<uint8_t>& a, const std::vector<uint8_t>& b);

    // Pad the input to be a multiple of 16 bytes (PKCS7 padding)
    std::vector<uint8_t> pad_input(const std::vector<uint8_t>& input);

    // Remove padding from the decrypted text
    std::vector<uint8_t> remove_padding(const std::vector<uint8_t>& padded_input);

    // 32bit�̃��[�h��4�o�C�g�̔z��ɕϊ�
    std::vector<uint8_t> word2ByteArray(uint32_t word);

    // 4�o�C�g�̔z���32bit�̃��[�h�ɕϊ�
    uint32_t byteArray2Word(const std::vector<uint8_t>& byteArray);

    // AES-NI��CPU�ɂ���̂����肷��v���O����
    bool check_aesni_support(const bool aesniflag);

private:
    // AES-NI���T�|�[�g����Ă��邩 true: �T�|�[�g����Ă��� false: �T�|�[�g����Ă��Ȃ�
    bool aesniSupported = false;

    const uint8_t AES_128 = 16;
    const uint8_t AES_192 = 24;
    const uint8_t AES_256 = 32;

    std::vector<uint8_t> key; // �Í��L�[

    const int Nb = 4; // �u���b�N�T�C�Y (���[�h�P��)
    int Nk = 0;       // �L�[�� (���[�h�P��)
    int Nr = 0;       // ���E���h��

    const uint8_t ivSize = 16;      // IV�̃T�C�Y (AES��16�o�C�g�̃u���b�N�T�C�Y������)
    const uint8_t paddingSize = 16; // �p�f�B���O�T�C�Y (PKCS7�p�f�B���O)

    // AES-NI�p�̌��X�P�W���[��
    std::vector<__m128i> rd_key;
    std::vector<__m128i> dec_key;

    std::vector<uint32_t> keyScheduleWords; // �\�t�g�E�F�A�����p�̌��X�P�W���[��
};