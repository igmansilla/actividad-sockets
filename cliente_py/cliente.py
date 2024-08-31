import socket

def main():
    HOST = '127.0.0.1'  
    PUERTO = 8080      

  
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PUERTO))
        print(f'Conectado al servidor en {HOST}:{PUERTO}')
        
        while True:
            try:
                datos = s.recv(1024)
                if not datos:
                    print("El servidor cerró la conexión.")
                    break

                try:
                    respuesta = datos.decode('utf-8')
                except UnicodeDecodeError:
                    respuesta = datos.decode('latin-1')

                print(f'Servidor: {respuesta}')

                if 'Seleccione una opción' in respuesta:
                    opcion = input('Elige una opción: (1) Generar nombre de usuario (2) Generar contraseña (3) Salir: ')
                    if opcion in ['1', '2', '3']:
                        s.sendall((opcion + '\n').encode('utf-8'))
                        if opcion == '3':
                            print("Desconectando...")
                            break
                    else:
                        print('Opción no válida.')
                        continue

                elif 'Ingrese la longitud' in respuesta:
                    try:
                        longitud = int(input('Ingresa la longitud deseada: '))
                        if longitud < 5 and 'Ingrese la longitud del nombre de usuario' in respuesta:
                            print('Longitud mínima es 5 para nombre de usuario.')
                            continue
                        if longitud < 8 and 'Ingrese la longitud de la contraseña' in respuesta:
                            print('Longitud mínima es 8 para contraseña.')
                            continue
                        s.sendall((str(longitud) + '\n').encode('utf-8'))
                    except ValueError:
                        print('Longitud inválida. Debe ser un número entero.')
                        continue

                else:
                    print(f'Resultado final: {respuesta}')

            except Exception as e:
                print(f'Error: {e}')
                break

if __name__ == "__main__":
    main()
