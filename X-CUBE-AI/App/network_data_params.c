/**
  ******************************************************************************
  * @file    network_data_params.c
  * @author  AST Embedded Analytics Research Platform
  * @date    Wed Dec 17 23:05:07 2025
  * @brief   AI Tool Automatic Code Generator for Embedded NN computing
  ******************************************************************************
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  */

#include "network_data_params.h"


/**  Activations Section  ****************************************************/
ai_handle g_network_activations_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(NULL),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};




/**  Weights Section  ********************************************************/
AI_ALIGNED(32)
const ai_u64 s_network_weights_array_u64[113] = {
  0x3f088ef5bf2c99b2U, 0x3f0c4a683e3662dfU, 0xbefedb63bee0eacfU, 0x3e84ddf53f0ede47U,
  0x3d8ce798be90e2d5U, 0xbed1fd163f26bc81U, 0x3f03adfebea7adb7U, 0x3ee49377bf3b865fU,
  0x3eff742ebedec5afU, 0x3ef096963e94f218U, 0xbe835f1fbe89088eU, 0xbec8df383e0267e6U,
  0xbef9cd6bbf42f26fU, 0x3e9b971e3f433a77U, 0xbe9d0f3bbe48f47bU, 0xbe93f6823ef2f5dcU,
  0xbc72bdf23c666965U, 0x3d42402a3e25453bU, 0x3d845128be8335c6U, 0x3e36fc103db8f3c4U,
  0xbdffb0b73c1f47e4U, 0x3def90edbdfc2efaU, 0x3d0b2b9bbe897df7U, 0x3d17b89d3d312514U,
  0xbedf1d313eb8c54fU, 0x3f1c644cbd41b855U, 0x3f1adc523ef05a1cU, 0x3def430bbede7deaU,
  0xbe717435be06fd78U, 0xbea68f063e4ca795U, 0x3f19ce943f3ba4feU, 0x3e9051603e4337afU,
  0xbddba990bd882d05U, 0x3eb323483e015c83U, 0xbf1fe7c7bf1e5e5aU, 0x3e557240bf672c2cU,
  0xbe004aa8bf85df01U, 0xbdb14ff6bef331d1U, 0xbcd762c03eb75db5U, 0xbeb028b53e2acde3U,
  0x3d9f7a20be7f1496U, 0xbf9b3f3c3efb688fU, 0xbeaea525be80ea2aU, 0xbe3b1e03bf1712faU,
  0xbe0bb45dbf99d0d6U, 0x3e353a37be337d46U, 0xbf794d8b3de1b69eU, 0x3f00867a3f12a1d8U,
  0x3f074dbbbf5749d1U, 0x3ea8d331bda99e31U, 0x3f0b93523ebef00aU, 0xbf4ad8d13d226853U,
  0xbe8da901bed90aafU, 0xbec3dc14bd7b19caU, 0x3ec83bbfbf14b145U, 0x3cd0d54bbecdcd3bU,
  0xbe303283bf0dc3b8U, 0x3ebcf0533e017fa4U, 0x3e1d9e33be0fbea7U, 0xbecc305ebc6d0380U,
  0xbdf84cfcbedcafc0U, 0xbe667beebe94cb02U, 0x3f05546c3e5ec4b7U, 0x3f16c2e93e66a48cU,
  0xbf20f69e3e5792a6U, 0x3f32ec713cce0aabU, 0x3dea23813ed1c2eaU, 0x3dd0c826bcbd6997U,
  0x3e37e2bdbe3ca8c7U, 0x3e0153473ee87035U, 0x3ebf69283f141598U, 0x3e507462bdb5ecc7U,
  0xbe818f2abf827cbeU, 0x3f6a0b5abda1c916U, 0x3e77a9fd3e843125U, 0xbed7620cbf2fc32fU,
  0x3e976c9ebf4d368dU, 0xbe2573fb3e87777bU, 0x3f337a363e03d2bbU, 0x3e6600273f0accceU,
  0xbd0901603e8c48a7U, 0x3ed8a2313ec41d52U, 0x3ef677e73e08ff9bU, 0xba912d7b3e0232b8U,
  0x3ef18a163dd82834U, 0x3e6b1ecc3e0fb95dU, 0x3eb353bc3e355b4aU, 0x3ec4c7ce3df46a27U,
  0x3fcb1ceb3e6c847cU, 0x3fbea816be993cfcU, 0x3d69e610be1e3906U, 0xbcdb9d5c3e944201U,
  0xbe869f88bcafc016U, 0xbea3efa9bd4d2494U, 0xbf32c1383f098c75U, 0x3f097f4e3e08e108U,
  0x3efffcd73f0dfc3aU, 0x3e9ffb5a3d92cc54U, 0xbf258d433f393ea6U, 0x3e9c9229bf230a02U,
  0x3f4c52d93f360688U, 0x3de69a7b3ecf33aeU, 0xbee5824cbec67939U, 0xbe4ec344be9c5c4cU,
  0xbd7bd1b03e62e4f8U, 0xbd4c7e503e052340U, 0xbf738f533f84a240U, 0x3fa28889be8c4344U,
  0xbd585a123f712d7fU, 0x3eb432653f0d8cefU, 0xbe1fab54be97f458U, 0xbe828db000000000U,
  0xbf7a0105U,
};


ai_handle g_network_weights_table[1 + 2] = {
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
  AI_HANDLE_PTR(s_network_weights_array_u64),
  AI_HANDLE_PTR(AI_MAGIC_MARKER),
};

