// This file is part of VkFFT
//
// Copyright (C) 2021 - present Dmitrii Tolmachev <dtolm96@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#ifndef VKFFT_CONSTANTS_H
#define VKFFT_CONSTANTS_H
#include "vkFFT_Structs/vkFFT_Structs.h"
#include "vkFFT_CodeGen/vkFFT_StringManagement/vkFFT_StringManager.h"
#include "vkFFT_CodeGen/vkFFT_MathUtils/vkFFT_MathUtils.h"

static inline void appendConstantsVkFFT(VkFFTSpecializationConstantsLayout* sc) {
	if (sc->res != VKFFT_SUCCESS) return;
	//appendConstant(sc, floatType, "loc_PI", "3.1415926535897932384626433832795", LFending);
	//
	//appendConstant(sc, floatType, "loc_SQRT1_2", "0.70710678118654752440084436210485", LFending);
	//
	VkContainer* uintType;
	VkGetTypeFromCode(sc, sc->uintTypeCode, &uintType);

	VkContainer* floatType;
	VkGetTypeFromCode(sc, sc->floatTypeCode, &floatType);
	if (sc->useRader) {
		for (int i = 0; i < sc->numRaderPrimes; i++) {
			if (sc->raderContainer[i].prime > 0) {
				if (sc->inline_rader_g_pow == 1) {
					int64_t g_pow = 1;
#if((VKFFT_BACKEND==3)||(VKFFT_BACKEND==4))
					sc->tempLen = sprintf(sc->tempStr, "__constant %s %s[%d]= {1", uintType->data.s, sc->raderContainer[i].g_powConstantStruct.data.s, sc->raderContainer[i].prime);
					VkAppendLine(sc);
					
#elif(VKFFT_BACKEND==5)
					sc->tempLen = sprintf(sc->tempStr, "constant %s %s[%d]= {1", uintType->data.s, sc->raderContainer[i].g_powConstantStruct.data.s, sc->raderContainer[i].prime);
					VkAppendLine(sc);
					
#else
					sc->tempLen = sprintf(sc->tempStr, "const %s %s[%d]= {1", uintType->data.s, sc->raderContainer[i].g_powConstantStruct.data.s, sc->raderContainer[i].prime);
					VkAppendLine(sc);
					
#endif
					for (int t = 0; t < sc->raderContainer[i].prime - 1; t++) {
						g_pow = (g_pow * sc->raderContainer[i].generator) % sc->raderContainer[i].prime;
						sc->tempLen = sprintf(sc->tempStr, ", %" PRIi64 "", g_pow);
						VkAppendLine(sc);
						
					}
					sc->tempLen = sprintf(sc->tempStr, "};\n");
					VkAppendLine(sc);
					
				}
				if (sc->inline_rader_kernel) {
#if((VKFFT_BACKEND==3)||(VKFFT_BACKEND==4))
					sc->tempLen = sprintf(sc->tempStr, "__constant %s %s[%d]= {", floatType->data.s, sc->raderContainer[i].r_rader_kernelConstantStruct.data.s, sc->raderContainer[i].prime - 1);
					VkAppendLine(sc);
					
#elif(VKFFT_BACKEND==5)
					sc->tempLen = sprintf(sc->tempStr, "constant %s %s[%d]= {", floatType->data.s, sc->raderContainer[i].r_rader_kernelConstantStruct.data.s, sc->raderContainer[i].prime - 1);
					VkAppendLine(sc);
					
#else
					sc->tempLen = sprintf(sc->tempStr, "const %s %s[%d]= {", floatType->data.s, sc->raderContainer[i].r_rader_kernelConstantStruct.data.s, sc->raderContainer[i].prime - 1);
					VkAppendLine(sc);
					
#endif
					if (sc->raderContainer[i].type == 0) {
						for (int j = 0; j < (sc->raderContainer[i].prime - 1); j++) {//fix later
							if (((sc->floatTypeCode % 100) / 10) == 2) {
								double* raderFFTKernel = (double*)sc->raderContainer[i].raderFFTkernel;
								sc->tempLen = sprintf(sc->tempStr, "%.17e", raderFFTKernel[2 * j] / (sc->raderContainer[i].prime - 1));
								VkAppendLine(sc);
								sc->tempLen = sprintf(sc->tempStr, "%s ", sc->doubleLiteral.data.s);
								VkAppendLine(sc);
							}
							if (((sc->floatTypeCode % 100) / 10) == 1) {
								float* raderFFTKernel = (float*)sc->raderContainer[i].raderFFTkernel;
								sc->tempLen = sprintf(sc->tempStr, "%.8e", raderFFTKernel[2 * j] / (sc->raderContainer[i].prime - 1));
								VkAppendLine(sc);
								sc->tempLen = sprintf(sc->tempStr, "%s ", sc->floatLiteral.data.s);
								VkAppendLine(sc);
							}
							if (j < (sc->raderContainer[i].prime - 2)) {
								sc->tempLen = sprintf(sc->tempStr, ", ");
								VkAppendLine(sc);
							}
							else {
								sc->tempLen = sprintf(sc->tempStr, "};\n");
								VkAppendLine(sc);								
							}
						}
					}
					else {
						for (int j = 0; j < (sc->raderContainer[i].prime - 1); j++) {//fix later
							uint64_t g_pow = 1;
							for (int t = 0; t < sc->raderContainer[i].prime - 1 - j; t++) {
								g_pow = (g_pow * sc->raderContainer[i].generator) % sc->raderContainer[i].prime;
							}
							if (((sc->floatTypeCode % 100) / 10) == 2) {
								double* raderFFTKernel = (double*)sc->raderContainer[i].raderFFTkernel;
								sc->tempLen = sprintf(sc->tempStr, "%.17e", (double)cos(2.0 * g_pow * sc->double_PI / sc->raderContainer[i].prime));
								VkAppendLine(sc);
								sc->tempLen = sprintf(sc->tempStr, "%s ", sc->doubleLiteral.data.s);
								VkAppendLine(sc);
							}
							if (((sc->floatTypeCode % 100) / 10) == 1) {
								float* raderFFTKernel = (float*)sc->raderContainer[i].raderFFTkernel;
								sc->tempLen = sprintf(sc->tempStr, "%.8e", (float)cos(2.0 * g_pow * sc->double_PI / sc->raderContainer[i].prime));
								VkAppendLine(sc);
								sc->tempLen = sprintf(sc->tempStr, "%s ", sc->floatLiteral.data.s);
								VkAppendLine(sc);
							}
							if (j < (sc->raderContainer[i].prime - 2)) {
								sc->tempLen = sprintf(sc->tempStr, ", ");
								VkAppendLine(sc);
								
							}
							else {
								sc->tempLen = sprintf(sc->tempStr, "};\n");
								VkAppendLine(sc);
								
							}
						}
					}
#if((VKFFT_BACKEND==3)||(VKFFT_BACKEND==4))
					sc->tempLen = sprintf(sc->tempStr, "__constant %s %s[%d]= {", floatType->data.s, sc->raderContainer[i].i_rader_kernelConstantStruct.data.s, sc->raderContainer[i].prime - 1);
					VkAppendLine(sc);
					
#elif(VKFFT_BACKEND==5)
					sc->tempLen = sprintf(sc->tempStr, "constant %s %s[%d]= {", floatType->data.s, sc->raderContainer[i].i_rader_kernelConstantStruct.data.s, sc->raderContainer[i].prime - 1);
					VkAppendLine(sc);
					
#else
					sc->tempLen = sprintf(sc->tempStr, "const %s %s[%d]= {", floatType->data.s, sc->raderContainer[i].i_rader_kernelConstantStruct.data.s, sc->raderContainer[i].prime - 1);
					VkAppendLine(sc);
					
#endif
					if (sc->raderContainer[i].type == 0) {
						for (int j = 0; j < (sc->raderContainer[i].prime - 1); j++) {//fix later
							if (((sc->floatTypeCode % 100) / 10) == 2) {
								double* raderFFTKernel = (double*)sc->raderContainer[i].raderFFTkernel;
								sc->tempLen = sprintf(sc->tempStr, "%.17e", raderFFTKernel[2 * j + 1] / (sc->raderContainer[i].prime - 1));
								VkAppendLine(sc);
								sc->tempLen = sprintf(sc->tempStr, "%s ", sc->doubleLiteral.data.s);
								VkAppendLine(sc);
							}
							if (((sc->floatTypeCode % 100) / 10) == 1) {
								float* raderFFTKernel = (float*)sc->raderContainer[i].raderFFTkernel;
								sc->tempLen = sprintf(sc->tempStr, "%.8e", raderFFTKernel[2 * j + 1] / (sc->raderContainer[i].prime - 1));
								VkAppendLine(sc);
								sc->tempLen = sprintf(sc->tempStr, "%s ", sc->floatLiteral.data.s);
								VkAppendLine(sc);
							}

							if (j < (sc->raderContainer[i].prime - 2)) {
								sc->tempLen = sprintf(sc->tempStr, ", ");
								VkAppendLine(sc);
								
							}
							else {
								sc->tempLen = sprintf(sc->tempStr, "};\n");
								VkAppendLine(sc);
								
							}
						}
					}
					else {
						for (int j = 0; j < (sc->raderContainer[i].prime - 1); j++) {//fix later
							uint64_t g_pow = 1;
							for (int t = 0; t < sc->raderContainer[i].prime - 1 - j; t++) {
								g_pow = (g_pow * sc->raderContainer[i].generator) % sc->raderContainer[i].prime;
							}
							if (((sc->floatTypeCode % 100) / 10) == 2) {
								double* raderFFTKernel = (double*)sc->raderContainer[i].raderFFTkernel;
								sc->tempLen = sprintf(sc->tempStr, "%.17e", (double)(-sin(2.0 * g_pow * sc->double_PI / sc->raderContainer[i].prime)));
								VkAppendLine(sc);
								sc->tempLen = sprintf(sc->tempStr, "%s ", sc->doubleLiteral.data.s);
								VkAppendLine(sc);
							}
							if (((sc->floatTypeCode % 100) / 10) == 1) {
								float* raderFFTKernel = (float*)sc->raderContainer[i].raderFFTkernel;
								sc->tempLen = sprintf(sc->tempStr, "%.8e", (float)(-sin(2.0 * g_pow * sc->double_PI / sc->raderContainer[i].prime)));
								VkAppendLine(sc);
								sc->tempLen = sprintf(sc->tempStr, "%s ", sc->floatLiteral.data.s);
								VkAppendLine(sc);
							}
							if (j < (sc->raderContainer[i].prime - 2)) {
								sc->tempLen = sprintf(sc->tempStr, ", ");
								VkAppendLine(sc);
								
							}
							else {
								sc->tempLen = sprintf(sc->tempStr, "};\n");
								VkAppendLine(sc);
								
							}
						}
					}
				}
			}
		}
	}
	return;
}

#endif
