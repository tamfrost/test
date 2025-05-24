FROM node:20

# Create app directory
WORKDIR /app

# Install dependencies (toolkit, etc.)
COPY package*.json ./
RUN npm install --production

# Copy the rest of your files
COPY index.js .

# Run the action
ENTRYPOINT ["node", "/app/index.js"]
