#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
    #define CLEAR_SCREEN() system("cls")
#else
    #include <unistd.h>
    #define CLEAR_SCREEN() system("clear")
#endif

#define MAX_ROOMS 50
#define MAX_NAME_LENGTH 50
#define MAX_TRANSACTIONS 100
#define ROOM_FILE "rooms.dat"
#define TRANSACTION_FILE "transactions.dat"

// Module 1: Room Data & Availability (Mahasiswa A)
typedef struct {
    int room_number;
    char type[20]; // "Standard", "Deluxe", "Suite"
    float price_per_night;
    int is_available;
    int max_guests;
} Room;

typedef struct {
    Room rooms[MAX_ROOMS];
    int room_count;
} RoomManager;

// Module 2: Booking & Cost Calculation (Mahasiswa B)
typedef struct {
    int booking_id;
    int room_number;
    char guest_name[MAX_NAME_LENGTH];
    char check_in_date[11]; // YYYY-MM-DD
    char check_out_date[11]; // YYYY-MM-DD
    int duration;
    float total_cost;
    int is_checked_out;
} Booking;

typedef struct {
    Booking bookings[MAX_TRANSACTIONS];
    int booking_count;
} BookingManager;

// Module 3: Data Storage & Reporting (Mahasiswa C)
typedef struct {
    RoomManager room_manager;
    BookingManager booking_manager;
} HotelSystem;

// Function declarations
void initialize_system(HotelSystem *hotel_system);
void display_main_menu();
void display_rooms(RoomManager *rm);
int check_availability(RoomManager *rm, int room_number);
void book_room(HotelSystem *hotel_system);
void calculate_cost(Booking *booking, Room *room);
void checkout_room(HotelSystem *hotel_system);
void save_rooms_to_file(RoomManager *rm);
void load_rooms_from_file(RoomManager *rm);
void save_transactions_to_file(BookingManager *bm);
void load_transactions_from_file(BookingManager *bm);
void generate_report(HotelSystem *hotel_system);
void press_any_key_to_continue();

// ############# MODUL 1 ###################
void initialize_rooms(RoomManager *rm) {
    // Initialize some sample rooms
    Room rooms[] = {
        {101, "Standard", 250000, 1, 2},
        {102, "Standard", 250000, 1, 2},
        {201, "Deluxe", 450000, 1, 3},
        {202, "Deluxe", 450000, 1, 3},
        {301, "Suite", 750000, 1, 4},
        {302, "Suite", 750000, 1, 4}
    };
    
    rm->room_count = 6;
    for(int i = 0; i < rm->room_count; i++) {
        rm->rooms[i] = rooms[i];
    }
}

void display_rooms(RoomManager *rm) {
    CLEAR_SCREEN();
    printf("\n=== DAFTAR KAMAR HOTEL ===\n");
    printf("%-8s\t%-12s\t%-16s\t%-10s\t%s\n", "No.Kamar", "Tipe", "Harga/Malam", "Status", "Max.Tamu");
    printf("-----------------------------------------------------------------------------\n");
    
    for(int i = 0; i < rm->room_count; i++) {
        Room room = rm->rooms[i];
        char status[20];
        if(room.is_available) {
            strcpy(status, "Tersedia");
        } else {
            strcpy(status, "Terisi");
        }
        
        printf("%-8d\t%-12s\tRp%-14.0f\t%-10s\t%d\n", 
               room.room_number, 
               room.type, 
               room.price_per_night, 
               status, 
               room.max_guests);
    }
    press_any_key_to_continue();
}

int check_availability(RoomManager *rm, int room_number) {
    for(int i = 0; i < rm->room_count; i++) {
        if(rm->rooms[i].room_number == room_number) {
            return rm->rooms[i].is_available;
        }
    }
    return 0;
}

// ############# MODUL 2 ###################
void book_room(HotelSystem *hotel_system) {
    int room_number;
    char guest_name[MAX_NAME_LENGTH];
    char check_in[11], check_out[11];
    
    CLEAR_SCREEN();
    printf("\n=== PEMESANAN KAMAR ===\n");
    
    // Show available rooms first
    printf("\n=== KAMAR YANG TERSEDIA ===\n");
    printf("No.Kamar\tTipe\t\tHarga/Malam\tMax.Tamu\n");
    printf("----------------------------------------------------------------\n");
    
    for(int i = 0; i < hotel_system->room_manager.room_count; i++) {
        Room room = hotel_system->room_manager.rooms[i];
        if(room.is_available) {
            printf("%d\t\t%s\t\tRp%.0f\t\t%d\n", 
                   room.room_number, 
                   room.type, 
                   room.price_per_night, 
                   room.max_guests);
        }
    }
    
    printf("\nMasukkan nomor kamar: ");
    scanf("%d", &room_number);
    
    // Check availability
    if(!check_availability(&hotel_system->room_manager, room_number)) {
        printf("Kamar %d tidak tersedia atau tidak ditemukan!\n", room_number);
        press_any_key_to_continue();
        return;
    }
    
    printf("Masukkan nama tamu: ");
    getchar(); // Clear buffer
    fgets(guest_name, MAX_NAME_LENGTH, stdin);
    guest_name[strcspn(guest_name, "\n")] = 0; // Remove newline
    
    printf("Masukkan tanggal check-in (YYYY-MM-DD): ");
    scanf("%s", check_in);
    
    printf("Masukkan tanggal check-out (YYYY-MM-DD): ");
    scanf("%s", check_out);
    
    // Find the room
    Room *selected_room = NULL;
    for(int i = 0; i < hotel_system->room_manager.room_count; i++) {
        if(hotel_system->room_manager.rooms[i].room_number == room_number) {
            selected_room = &hotel_system->room_manager.rooms[i];
            break;
        }
    }
    
    if(selected_room == NULL) {
        printf("Kamar tidak ditemukan!\n");
        press_any_key_to_continue();
        return;
    }
    
    // Create booking
    Booking new_booking;
    new_booking.booking_id = hotel_system->booking_manager.booking_count + 1;
    new_booking.room_number = room_number;
    strcpy(new_booking.guest_name, guest_name);
    strcpy(new_booking.check_in_date, check_in);
    strcpy(new_booking.check_out_date, check_out);
    new_booking.duration = 1; // Simplified duration calculation
    new_booking.is_checked_out = 0;
    
    calculate_cost(&new_booking, selected_room);
    
    // Add to bookings
    hotel_system->booking_manager.bookings[hotel_system->booking_manager.booking_count] = new_booking;
    hotel_system->booking_manager.booking_count++;
    
    // Update room availability
    selected_room->is_available = 0;
    
    CLEAR_SCREEN();
    printf("\n=== PEMESANAN BERHASIL ===\n");
    printf("ID Booking: %d\n", new_booking.booking_id);
    printf("Nama Tamu: %s\n", new_booking.guest_name);
    printf("Nomor Kamar: %d\n", new_booking.room_number);
    printf("Tipe Kamar: %s\n", selected_room->type);
    printf("Check-in: %s\n", new_booking.check_in_date);
    printf("Check-out: %s\n", new_booking.check_out_date);
    printf("Total biaya: Rp%.0f\n", new_booking.total_cost);
    
    press_any_key_to_continue();
}

void calculate_cost(Booking *booking, Room *room) {
    // Simple calculation: price * duration
    // In a real system, you'd calculate actual days between dates
    booking->total_cost = room->price_per_night * booking->duration;
}

void checkout_room(HotelSystem *hotel_system) {
    int booking_id;
    
    CLEAR_SCREEN();
    printf("\n=== CHECKOUT KAMAR ===\n");
    
    // Show active bookings
    printf("=== BOOKING AKTIF ===\n");
    printf("ID\tKamar\tNama Tamu\tCheck-In\tCheck-Out\n");
    printf("--------------------------------------------------------\n");
    
    int active_bookings = 0;
    for(int i = 0; i < hotel_system->booking_manager.booking_count; i++) {
        if(!hotel_system->booking_manager.bookings[i].is_checked_out) {
            Booking booking = hotel_system->booking_manager.bookings[i];
            printf("%d\t%d\t%s\t%s\t%s\n", 
                   booking.booking_id,
                   booking.room_number,
                   booking.guest_name,
                   booking.check_in_date,
                   booking.check_out_date);
            active_bookings++;
        }
    }
    
    if(active_bookings == 0) {
        printf("Tidak ada booking aktif.\n");
        press_any_key_to_continue();
        return;
    }
    
    printf("\nMasukkan ID Booking: ");
    scanf("%d", &booking_id);
    
    // Find booking
    Booking *booking = NULL;
    for(int i = 0; i < hotel_system->booking_manager.booking_count; i++) {
        if(hotel_system->booking_manager.bookings[i].booking_id == booking_id && 
           !hotel_system->booking_manager.bookings[i].is_checked_out) {
            booking = &hotel_system->booking_manager.bookings[i];
            break;
        }
    }
    
    if(booking == NULL) {
        printf("Booking ID tidak ditemukan atau sudah checkout!\n");
        press_any_key_to_continue();
        return;
    }
    
    // Find and update room
    for(int i = 0; i < hotel_system->room_manager.room_count; i++) {
        if(hotel_system->room_manager.rooms[i].room_number == booking->room_number) {
            hotel_system->room_manager.rooms[i].is_available = 1;
            break;
        }
    }
    
    booking->is_checked_out = 1;
    
    CLEAR_SCREEN();
    printf("\n=== CHECKOUT BERHASIL ===\n");
    printf("ID Booking: %d\n", booking->booking_id);
    printf("Nama Tamu: %s\n", booking->guest_name);
    printf("Kamar: %d\n", booking->room_number);
    printf("Check-in: %s\n", booking->check_in_date);
    printf("Check-out: %s\n", booking->check_out_date);
    printf("Total yang dibayar: Rp%.0f\n", booking->total_cost);
    
    press_any_key_to_continue();
}

// ############# MODUL 3 ###################
void save_rooms_to_file(RoomManager *rm) {
    FILE *file = fopen(ROOM_FILE, "wb");
    if(file == NULL) {
        printf("Error menyimpan data kamar!\n");
        return;
    }
    
    fwrite(&rm->room_count, sizeof(int), 1, file);
    fwrite(rm->rooms, sizeof(Room), rm->room_count, file);
    fclose(file);
}

void load_rooms_from_file(RoomManager *rm) {
    FILE *file = fopen(ROOM_FILE, "rb");
    if(file == NULL) {
        // If file doesn't exist, initialize with default rooms
        initialize_rooms(rm);
        return;
    }
    
    fread(&rm->room_count, sizeof(int), 1, file);
    fread(rm->rooms, sizeof(Room), rm->room_count, file);
    fclose(file);
}

void save_transactions_to_file(BookingManager *bm) {
    FILE *file = fopen(TRANSACTION_FILE, "wb");
    if(file == NULL) {
        printf("Error menyimpan data transaksi!\n");
        return;
    }
    
    fwrite(&bm->booking_count, sizeof(int), 1, file);
    fwrite(bm->bookings, sizeof(Booking), bm->booking_count, file);
    fclose(file);
}

void load_transactions_from_file(BookingManager *bm) {
    FILE *file = fopen(TRANSACTION_FILE, "rb");
    if(file == NULL) {
        bm->booking_count = 0;
        return;
    }
    
    fread(&bm->booking_count, sizeof(int), 1, file);
    fread(bm->bookings, sizeof(Booking), bm->booking_count, file);
    fclose(file);
}

void generate_report(HotelSystem *hotel_system) {
    CLEAR_SCREEN();
    printf("\n=== LAPORAN TRANSAKSI ===\n");
    printf("ID\tKamar\tNama Tamu\tCheck-In\tCheck-Out\tTotal\tStatus\n");
    printf("----------------------------------------------------------------------------\n");
    
    int total_bookings = 0;
    int active_bookings = 0;
    float total_revenue = 0;
    
    for(int i = 0; i < hotel_system->booking_manager.booking_count; i++) {
        Booking booking = hotel_system->booking_manager.bookings[i];
        char status[20];
        if(booking.is_checked_out) {
            strcpy(status, "Selesai");
            total_revenue += booking.total_cost;
        } else {
            strcpy(status, "Aktif");
            active_bookings++;
        }
        total_bookings++;
        
        printf("%d\t%d\t%s\t%s\t%s\tRp%.0f\t%s\n", 
               booking.booking_id,
               booking.room_number,
               booking.guest_name,
               booking.check_in_date,
               booking.check_out_date,
               booking.total_cost,
               status);
    }
    
    printf("\n=== RINGKASAN ===\n");
    printf("Total Booking: %d\n", total_bookings);
    printf("Booking Aktif: %d\n", active_bookings);
    printf("Total Pendapatan: Rp%.0f\n", total_revenue);
    
    press_any_key_to_continue();
}

// Utility functions
void press_any_key_to_continue() {
    printf("\nTekan Enter untuk melanjutkan...");
    getchar(); // Clear previous newline
    getchar(); // Wait for Enter key
}

// Main system functions
void initialize_system(HotelSystem *hotel_system) {
    load_rooms_from_file(&hotel_system->room_manager);
    load_transactions_from_file(&hotel_system->booking_manager);
}

void display_main_menu() {
    CLEAR_SCREEN();
    printf("\n=== SISTEM RESERVASI HOTEL ===\n");
    printf("================================\n");
    printf("1. Lihat Daftar Kamar\n");
    printf("2. Cek Ketersediaan Kamar\n");
    printf("3. Pesan Kamar\n");
    printf("4. Checkout\n");
    printf("5. Laporan Transaksi\n");
    printf("6. Simpan Data\n");
    printf("7. Keluar\n");
    printf("================================\n");
    printf("Pilih menu (1-7): ");
}

int main() {
    HotelSystem hotel;
    int choice;
    
    initialize_system(&hotel);
    
    do {
        display_main_menu();
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:
                display_rooms(&hotel.room_manager);
                break;
            case 2:
                {
                    int room_number;
                    CLEAR_SCREEN();
                    printf("\n=== CEK KETERSEDIAAN KAMAR ===\n");
                    printf("Masukkan nomor kamar: ");
                    scanf("%d", &room_number);
                    if(check_availability(&hotel.room_manager, room_number)) {
                        printf("Kamar %d TERSEDIA!\n", room_number);
                    } else {
                        printf("Kamar %d TIDAK TERSEDIA!\n", room_number);
                    }
                    press_any_key_to_continue();
                }
                break;
            case 3:
                book_room(&hotel);
                break;
            case 4:
                checkout_room(&hotel);
                break;
            case 5:
                generate_report(&hotel);
                break;
            case 6:
                save_rooms_to_file(&hotel.room_manager);
                save_transactions_to_file(&hotel.booking_manager);
                CLEAR_SCREEN();
                printf("\nData berhasil disimpan!\n");
                press_any_key_to_continue();
                break;
            case 7:
                CLEAR_SCREEN();
                printf("\nTerima kasih telah menggunakan Sistem Reservasi Hotel!\n");
                break;
            default:
                printf("Pilihan tidak valid!\n");
                press_any_key_to_continue();
        }
    } while(choice != 7);
    
    return 0;
}