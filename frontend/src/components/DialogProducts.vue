<template>
    <v-dialog
        v-model="isOpen"
    >
        <template v-slot:activator="{on, attrs}">
            <v-btn
                target="_blank"
                text
                v-bind="attrs"
                v-on="on"
            >
                <span class="mr-2">Producten</span>
            </v-btn>
        </template>
        <v-card>
            <v-data-table
                :headers="headers"
                :items="items"
                :items-per-page="25"
                :loading="isLoading"
                dense
            >
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
                <template v-slot:[`item.name`]="props">
                    <v-edit-dialog
                        :return-value.sync="props.item.name"
                        @save="save(props.item)"
                    >
                        <div v-if="props.item.name">{{ props.item.name }}</div>
                        <div v-else><i>Onbekend Product</i></div>
                        <template v-slot:input>
                            <v-text-field
                                v-model="props.item.name"
                                label="Naam bewerken"
                                single-line
                            ></v-text-field>
                        </template>
                    </v-edit-dialog>
                </template>
                <template v-slot:[`item.price`]="props">
                    <v-edit-dialog
                        :return-value.sync="props.item.price"
                        @save="save(props.item)"
                    >
                        <div v-if="props.item.name">{{ props.item.price }}</div>
                        <div v-else><i>Onbekende Prijs</i></div>
                        <template v-slot:input>
                            <v-text-field
                                v-model="props.item.price"
                                label="Prijs bewerken"
                                single-line
                                type="number"
                                steps="0.01"
                            ></v-text-field>
                        </template>
                    </v-edit-dialog>
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
            <v-divider></v-divider>

            <v-card-actions>
                <v-spacer></v-spacer>
                <v-btn
                    color="primary"
                    text
                    @click="isOpen = false"
                >
                    Sluiten
                </v-btn>
            </v-card-actions>
        </v-card>
    </v-dialog>
</template>
<script>

export default {
    data() {
        return {
            isOpen: false,
            headers: [
                {
                    text: 'Id',
                    value: 'nfcId',
                },
                {
                    text: 'Naam',
                    value: 'name',
                },
                {
                    text: 'Prijs',
                    value: 'price',
                },
                {
                    text: 'Acties',
                    value: 'actions',
                    sortable: false,
                }
            ],
            items: [],
            isLoading: false,
        };
    },
    mounted() {
        this.reload()
    },
    methods: {
        deleteItem(item) {
            fetch(`http://localhost:3000/item/${item.nfcId}`, {
                method: 'DELETE'
            })
                .then(() => {
                    this.items = this.items.filter(arrayItem => arrayItem.nfcId !== item.nfcId)
                })
                .catch(console.log)
        },
        deleteAll() {
            fetch(`http://localhost:3000/items`, {
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
            fetch('http://localhost:3000/items')
                .then(response => response.json())
                .then(items => {
                    this.items = items
                    this.isLoading = false
                })
                .catch((err) => {
                    console.log(err)
                    this.isLoading = false
                })
        },
        save(item) {
            fetch(`http://localhost:3000/item/${item.nfcId}`, {
                method: 'PATCH',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(item)
            })
                .catch(console.log)
        }
    }
}
</script>