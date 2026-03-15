import DefaultTheme from 'vitepress/theme'
import './custom.css'
import NandGate from '../components/NandGate.vue'

export default {
    ...DefaultTheme,
    enhanceApp({ app }) {
        app.component('NandGate', NandGate)
    }
}