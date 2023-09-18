#ifndef MOVEMENT_H
#define MOVEMENT_H

void randomMovementString(int location, bool insulting, int inputRow);
void randomMovementSound(ma_engine *engine);
void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

#endif