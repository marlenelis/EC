##############################################
# file: suma64sgn.s
# compile: 	gcc -m32    suma64sgn.s -o suma
# (ddd)		gcc -m32 -g suma64sgn.s -o suma
##############################################

.section .data
	.macro linea
		#	.int -1,-1,-1,-1
		#	.int 0xffffffff,0xffffffff,0xffffffff,0xffffffff
		#	.int  1,-2, 1,-2
		#	.int  1,2,-3,-4
		#	.int 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF
		#	.int 0x80000000, 0x08000000, 0x08000000, 0x08000000
		#	.int 0x04000000, 0x04000000, 0x04000000, 0x04000000
		#	.int 0x08000000, 0x08000000, 0x08000000, 0x08000000
			.int 0xFC000000, 0xFC000000, 0xFC000000, 0xFC000000
		#	.int 0xF8000000, 0xF8000000, 0xF8000000, 0xF8000000
		#	.int 0xF0000000, 0xE0000000, 0xE0000000, 0xD0000000

	.endm

lista:	.irpc i,12345678
		linea
	.endr

#lista: .int -1,1

longlista:	.int (.-lista)/4
resultado:	.quad 0x0123456789ABCDF

formato:
	.ascii "suma = %lld = %llx hex\n\0" #formato para 64 bits

.section .text
#_start:	.global _start

main: .global main

	mov $lista, %ebx  
	mov longlista, %ecx
	call suma
	mov %eax, resultado
	mov %edx , resultado+4  #64 little-endian

	push resultado+4
	push resultado
	push resultado+4
	push resultado

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
	cltd          #extension de signo
	add %eax, %edi 
	adc %edx, %ebp

	inc %esi #incrementa esi
	cmp %esi, %ecx	
	jne bucle # salta a la etiqueta bucle: mientras no sean iguales esi y ecx

	mov %edi, %eax
	mov %ebp, %edx
	pop %esi
	ret
