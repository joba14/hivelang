
BITS 64

segment .text

;; -- main -- 
global _start
_start:
	mov [args_ptr], rsp
	mov rax, ret_stack_end
	mov [ret_stack_rsp], rax
;; -- Hello, World!\n -- 
addr_5bc827d0a00944718fbea04ff553f5ad2a5aa814e4e9bb9626f4af2c44bdcefd:
	mov rax, 14
	push rax
	push str_5de3c4c5877df3e6a57400345f067b1a41f3c3f15eec324c12f80bb316c9ec51
;; -- 1 -- 
addr_154e3a36228412720f1bd8c095d856940c122e0675fa767bc1ce552fc1e00398:
	mov rax, 1
	push rax
;; -- 1 -- 
addr_c85d320b2b69e2cc0f955b7bafdce32a14f957ae1ff728563309695bc508e2b8:
	mov rax, 1
	push rax
;; -- syscall3 -- 
addr_6f440309fd561e491e4c00c48ccbf4388653e24ac5e8c4ac3aefcbd27e4fb377:
	pop rax
	pop rdi
	pop rsi
	pop rdx
	syscall
	push rax
;; -- drop -- 
addr_63c187d567c72b1f40abde03a17c5465426f0f076f2dc3eab034a2114ca72189:
	pop rax
;; -- 0 -- 
addr_8b42ee0aff3eaab7e25c18e0dc043a92b67c6fc1471b8856b3dadbfb1ed1ab0c:
	mov rax, 0
	push rax
;; -- end -- 
	mov rax, 60
	mov rdi, 0
	syscall

segment .data
	str_5de3c4c5877df3e6a57400345f067b1a41f3c3f15eec324c12f80bb316c9ec51: db 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0xa

segment .bss
	args_ptr: resq 1
	ret_stack_rsp: resq 1
	ret_stack: resb 4096
	ret_stack_end:
