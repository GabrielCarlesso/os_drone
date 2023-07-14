import 'dart:io';

void main(List<String> arguments) {
int port = 0;
  if (arguments.length > 0) {
	port = int.parse(arguments[0]);
  } else {
    print('Nenhum argumento foi passado.');
    return;
  }
  
  Socket.connect('localhost', port).then((socket) {
    print('Conectado ao servidor.');

    socket.listen(
      (data) {
        print('Resposta do servidor: ${String.fromCharCodes(data)}');
      },
      onError: (error) {
        print('Erro: $error');
        socket.destroy();
      },
      onDone: () {
        print('Desconectado do servidor.');
        socket.destroy();
      },
    );

    // Enviar uma solicitação ao servidor com a mensagem fornecida pelo usuário
    stdin.listen((data) {
      String mensagem = String.fromCharCodes(data).trim();
      socket.write(mensagem);
      print('Enviado para o servidor: $mensagem');
    });
  }).catchError((error) {
    print('Erro ao conectar ao servidor: $error');
  });
}
