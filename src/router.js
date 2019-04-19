import React from 'react';
import { BrowserRouter, Route } from 'react-router-dom';

// Layours
import MainLayout from './components/layout/main-layout';

export default (
   <BrowserRouter>
      <Route component={MainLayout}>

      </Route>
   </BrowserRouter>
)