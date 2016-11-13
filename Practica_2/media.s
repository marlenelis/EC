##############################################
# file: media.s
# compile: 	gcc -m32    media.s -o media
# (ddd)		gcc -m32 -g media.s -o media
##############################################

.section .data
	.macro linea
		#	.int 1, -2,  1, -2
		#	.int 1,  2, -3, -4
		#	.int 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF
		#	.int 0x80000000, 0x80000000, 0x80000000, 0x80000000
		#	.int 0xF0000000, 0xE0000000, 0xE0000000, 0xD0000000
			.int  -1, -1, -1, -1

	.endm

	.macro linea0
		#	.int   0, -1, -1, -1
		#	.int   0, -2, -1, -1
			.int   1, -2, -1, -1
		#	.int  64, -2, -1, -1
		#	.int -31, -2, -1, -1
		#	.int -20, -2, -1, -1
	.endm

lista: linea0
	.irpc i, 1234567#8
		linea
	.endr

longlista:	.int (.-lista)/4

media: .int 0x89ABCDF
resto: .int 0x01234567

formato:
	.ascii "media = %8d \tresto = %8d \nhex   = 0x%08x\tresto= 0x%08x\n \0" #formato para 64 bits

.section .text

main: .global main
	mov $lista, %ebx
	mov longlista, %ecx
	call suma
	
	mov %eax, media
	mov %edx, resto  #64 little-endian


	push resto
	push media
	push resto
	push media
	push $formato

	call printf 
	add $20 , %esp

	mov $1, %eax
	mov $0, %ebx
	int $0x80

suma:
	push %esi	
	mov $0, %ebp
	mov $0, %edi
	mov $0, %esi #iterador desde nuestra subrutina

bucle:
	mov  (%ebx,%esi,4), %eax
	cltd          	#extension de signo
	add %eax, %edi
	adc %edx, %ebp  # sumar con acarreo

	inc %esi
	cmp %esi, %ecx	
	jne bucle

	mov %edi, %eax
	mov %ebp, %edx
	pop %esi

	idiv %ecx
	ret
