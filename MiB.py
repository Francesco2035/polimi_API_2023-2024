def bytes_to_mib(bytes_size):
    """
    Convert bytes to MiB (Mebibytes).
    
    :param bytes_size: The size in bytes.
    :return: The size in MiB.
    """
    MIB_FACTOR = 2 ** 20  # 1 MiB = 2^20 Bytes
    mib_size = bytes_size / MIB_FACTOR
    return mib_size

def main():
    # Chiedi all'utente di inserire la dimensione in Byte
    try:
        bytes_size = int(input("Inserisci la dimensione in Byte: "))
    except ValueError:
        print("Per favore, inserisci un numero intero valido.")
        return

    # Converti la dimensione in MiB
    mib_size = bytes_to_mib(bytes_size)

    # Stampa il risultato
    print(f"La dimensione in MiB è: {mib_size:.6f} MiB")

if __name__ == "__main__":
    main()
