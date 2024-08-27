import socket

def main():
    HOST = '127.0.0.1'  # Dirección IP del servidor
    PORT = 8080          # Puerto del servidor

    # Crear un socket TCP/IP
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        print(f'Conectado al servidor en {HOST}:{PORT}')
        
        while True:
            try:
                # Recibir datos del servidor
                data = s.recv(1024)
                if not data:
                    break

                # Intentar decodificar los datos recibidos
                try:
                    response = data.decode('utf-8')
                except UnicodeDecodeError:
                    response = data.decode('latin-1')  # Intentar con una codificación diferente

                print(f'Servidor: {response}')

                if 'Seleccione una opción' in response:
                    opcion = input('Elige una opción: (1) Generar nombre de usuario (2) Generar contraseña: ')
                    if opcion == '1' or opcion == '2':
                        s.sendall((opcion + '\n').encode('utf-8'))
                    else:
                        print('Opción no válida.')
                        continue

                elif 'Ingrese la longitud' in response:
                    longitud = input('Ingresa la longitud deseada: ')
                    s.sendall((longitud + '\n').encode('utf-8'))

                else:
                    # Mostrar la respuesta final y salir
                    print(f'Resultado final: {response}')
                    break

            except Exception as e:
                print(f'Error: {e}')
                break

if __name__ == "__main__":
    main()
