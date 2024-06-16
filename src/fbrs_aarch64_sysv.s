.type fbrs_save_context, @function
.global fbrs_save_context
fbrs_save_context:
  # store link register (return address)
  str lr, [x0, 0]

  # save stack pointer
  mov x9, sp
  str x9, [x0, 8]

  # save calle preserved general purpose registers
  stp x19, x20, [x0, 16]
  stp x21, x22, [x0, 32]
  stp x23, x24, [x0, 48]
  stp x25, x26, [x0, 64]
  stp x27, x28, [x0, 80]
  str x29,      [x0, 88]

  # save calle preserved float registers
  stp  d8,  d9, [x0, 96]
  stp d10, d11, [x0, 112]
  stp d12, d13, [x0, 128]
  stp d14, d15, [x0, 144]

  mov x0, 0
  ret


.type fbrs_load_context, @function
.global fbrs_load_context
fbrs_load_context:
  # load link register (return address)
  ldr lr, [x0]

  # load stack pointer
  ldr x9, [x0, 8]
  mov sp, x9

  # load calle preserved general purpose registers
  ldp x19, x20, [x0, 16]
  ldp x21, x22, [x0, 32]
  ldp x23, x24, [x0, 48]
  ldp x25, x26, [x0, 64]
  ldp x27, x28, [x0, 80]
  ldr x29,      [x0, 88]

  # load calle preserved float registers
  ldp  d8,  d9, [x0, 96]
  ldp d10, d11, [x0, 112]
  ldp d12, d13, [x0, 128]
  ldp d14, d15, [x0, 144]

  mov x0, 0
  ret

.type fbrs_switch_context, @function
.global fbrs_switch_context
fbrs_switch_context:
  # store link register (return address)
  str lr, [x0, 0]

  # save stack pointer
  mov x9, sp
  str x9, [x0, 8]

  # save calle preserved general purpose registers
  stp x19, x20, [x0, 16]
  stp x21, x22, [x0, 32]
  stp x23, x24, [x0, 48]
  stp x25, x26, [x0, 64]
  stp x27, x28, [x0, 80]
  str x29,      [x0, 88]

  # save calle preserved float registers
  stp  d8,  d9, [x0, 96]
  stp d10, d11, [x0, 112]
  stp d12, d13, [x0, 128]
  stp d14, d15, [x0, 144]

  # load link register (return address)
  ldr lr, [x1]

  # load stack pointer
  ldr x9, [x1, 8]
  mov sp, x9

  # load calle preserved general purpose registers
  ldp x19, x20, [x1, 16]
  ldp x21, x22, [x1, 32]
  ldp x23, x24, [x1, 48]
  ldp x25, x26, [x1, 64]
  ldp x27, x28, [x1, 80]
  ldr x29,      [x1, 88]

  # load calle preserved float registers
  ldp  d8,  d9, [x1, 96]
  ldp d10, d11, [x1, 112]
  ldp d12, d13, [x1, 128]
  ldp d14, d15, [x1, 144]

  mov x0, 0
  ret
