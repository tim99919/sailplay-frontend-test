import React from 'react';
// import { Link } from 'react-router-dom';

export default props => {
   return (
      <div class="content_wrapper">
         <div class="content_wrapper_inner">
            <div class="route_title">

               <h1>Клиенты</h1>

            </div>
            <main>
               {props.children}
            </main>
         </div>
      </div>
   )
}