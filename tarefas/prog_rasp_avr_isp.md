## Configuração Raspberry PI 2 como ISP AVR Programmer
<span style="color:red">ATENÇÃO, GRAVAR O CÓDIGO ATRAVÉS DO ISP APAGA O BOOTLOADER DO ARDUINO/AVR!
</span> <br />Para recuperar o bootloader, siga esse [link](https://github.com/Bianorz/gynga/blob/master/tarefas/install_bootloader.md). 


### Adicionar repositório da Adafruit
Essa adição não é mandatória, porém é recomendada para ter certeza que a versão dos programas instalados aqui serão as mesmas que as de qualquer pessoa seguindo este tutorial.

- curl -sLS https://apt.adafruit.com/add | sudo bash
### Instalar o avrdude
Esse será o programa responsável pela gravação dos arquivos no microcontrolador

- sudo apt-get install avrdude
### Verificar instalação
- avrdude -v
### Conexão de cabos entre Raspberry e Arduino/AVR
 Nesse exemplo faremos a seguinte conexão:

- Arduino ICSP VCC para Raspberry Pi 5 volt pin.
- Arduino ICSP GND para Raspberry Pi ground pin.
- Arduino ICSP RESET para Raspberry Pi GPIO #12.
- Arduino ICSP SCK para Raspberry Pi GPIO #24.
- Arduino ICSP MOSI para Raspberry Pi GPIO #23.
- Arduino ICSP MISO para Raspberry Pi GPIO #18.
### Edição do arquivo de configuração do programador
O arquivo abaixo deve ser editado para adicionar as configurações que serão utilizadas pelo raspberry pi para gravação.

- cp /etc/avrdude.conf ~/avrdude_gpio.conf <br />
- nano ~/avrdude_gpio.conf

 Quando o editor abrir o arquivo, desça até o final dele e adicione o código abaixo
 
 
    # Linux GPIO configuration for avrdude.
    # Change the lines below to the GPIO pins connected to the AVR.
    programmer
      id    = "pi_1";
      desc  = "Use the Linux sysfs interface to bitbang GPIO lines";
      type  = "linuxgpio";
      reset = 12;
      sck   = 24;
      mosi  = 23;
      miso  = 18;
    ;
 
Com isso o programador está configurado, você pode realizar um teste da programação seguindo este [link](https://github.com/Bianorz/gynga/blob/master/tarefas/comp_c_hex.md).
