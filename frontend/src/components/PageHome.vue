<template>
    <v-container>
        <v-data-table
            :headers="headers"
            :items="items"
            :items-per-page="25"
            :loading="isLoading"
            dense
        >
            <v-dialog max-width="500px">
                <v-card>
                    <v-card-title class="text-h5">Are you sure you want to delete this item?</v-card-title>
                    <v-card-actions>
                    <v-spacer></v-spacer>
                    <v-btn color="blue darken-1" text >Cancel</v-btn>
                    <v-btn color="blue darken-1" text >OK</v-btn>
                    <v-spacer></v-spacer>
                    </v-card-actions>
                </v-card>
            </v-dialog>
            <template v-slot:top>
                <v-toolbar flat>
                    <v-toolbar-title>Gescande Producten</v-toolbar-title>
                    <v-spacer />
                    <v-btn
                        elevation="0"
                        color="error"
                        class="mr-2"
                        @click="deleteAll()"
                    >
                        <v-icon>
                            mdi-delete
                        </v-icon>
                    </v-btn>
                    <v-btn
                        elevation="0"
                        color="success"
                        @click="reload()"
                    >
                        <v-icon>
                            mdi-reload
                        </v-icon>
                    </v-btn>
                </v-toolbar>
            </template>
            <template v-slot:[`item.actions`]="{ item }">
                <v-icon
                    small
                    class="mr-2"
                    @click="deleteItem(item)"
                >
                    mdi-delete
                </v-icon>
            </template>
        </v-data-table>
    </v-container>
</template>
<script>
export default {
    data() {
        return {
            headers: [
                {
                    text: 'Apparaat',
                    value: 'scanner',
                },
                {
                    text: 'Product',
                    value: 'item',
                },
                {
                    text: 'Prijs',
                    value: 'price',
                },
                {
                    text: 'Tijdstip',
                    value: 'time',
                },
                {
                    text: 'Acties',
                    value: 'actions',
                    sortable: false,
                }
            ],
            items: [],
            isLoading: false,
        }
    },
    mounted() {
        this.reload()
    },
    methods: {
        deleteItem(item) {
            fetch(`http://localhost:3000/scan/${item.org._id}`, {
                method: 'DELETE'
            })
                .then(() => {
                    this.items = this.items.filter(arrayItem => arrayItem.org._id !== item.org._id)
                })
                .catch(console.log)
        },
        deleteAll() {
            fetch(`http://localhost:3000/scan`, {
                method: 'DELETE'
            })
                .then(() => {
                    this.items = []
                })
                .catch(console.log)
        },
        reload() {
            this.isLoading = true;
            this.items = [];
            fetch('http://localhost:3000/scan')
                .then(response => response.json())
                .then(data => data.map(scan => ({
                    org: scan,
                    scanner: scan.scanner ? scan.scanner.name || scan.scanner.address : 'Verwijderde Scanner',
                    item: scan.item ? scan.item.name || scan.scanner.nfcId : 'Verwijderd Product',
                    price: scan.item ? scan.item.price : 'Verwijderd Product',
                    time: new Date(scan.createdAt).toLocaleString()
                })))
                .then(items => {
                    this.items = items
                    this.isLoading = false
                })
                .catch((err) => {
                    console.log(err)
                    this.isLoading = false
                })
        }
    }
}
</script>