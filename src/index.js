import React from 'react';
import { Provider } from 'react-redux';
import store from './store';
import router from './router';
import ReactDOM from 'react-dom';

ReactDOM.render(
   <Provider store={store}>{router}</Provider>,
   document.getElementById('root')
);
