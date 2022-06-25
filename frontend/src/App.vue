<template>
    <v-app>
        <v-app-bar
            app
            color="primary"
            dark
        >
          <v-spacer></v-spacer>
          <DialogProducts />
        </v-app-bar>

        <v-main>
            <PageHome
                @closeDialog="dialogProductsOpen = false"
            />
        </v-main>
        <ConfettiElement />
    </v-app>
</template>

<script>
import ConfettiElement from './components/ConfettiElement'
import PageHome from './components/PageHome'
import DialogProducts from './components/DialogProducts'

export default {
    name: 'App',

    components: {
        PageHome,
        ConfettiElement,
        DialogProducts,
    },
};
</script>
